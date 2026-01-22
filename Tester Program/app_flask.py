
from flask import Flask, render_template, request, jsonify
import os
import subprocess
import requests
import json
import difflib
import re
from datetime import datetime
from pypdf import PdfReader
from io import BytesIO
from werkzeug.utils import secure_filename
import tempfile
import shutil
import traceback

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'uploads'
app.config['MAX_CONTENT_LENGTH'] = 16 * 1024 * 1024
app.config['SECRET_KEY'] = 'fdp-autocorrezione-2026'

os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)
os.makedirs('reports', exist_ok=True)

test_history = []
test_cache = {}

class OllamaAnalyzer:
    def __init__(self):
        self.base_url = "http://localhost:11434"
        self.model = "qwen2.5-coder:7b"
        self.available = self.check_availability()

    def check_availability(self):
        try:
            response = requests.get(f"{self.base_url}/api/tags", timeout=2)
            if response.status_code == 200:
                models = response.json().get('models', [])
                for model in models:
                    if self.model in model.get('name', ''):
                        print(f"[AI] ✅ Ollama available with {self.model}")
                        return True
                print(f"[AI] ⚠️ Ollama found but {self.model} not installed")
                return False
        except Exception as e:
            print(f"[AI] ❌ Ollama not available: {e}")
            return False

    def analyze_terza_parte(self, user_code, pdf_text, output):
        """Analizza il codice per verificare implementazione TERZA PARTE"""
        if not self.available:
            return None

        try:
            prompt = f"""Sei un assistente per correzione esami di C++. 

COMPITO: Verifica se la TERZA PARTE è implementata correttamente nel codice studente.

CODICE STUDENTE:
```cpp
{user_code[:3000]}
```

TESTO PDF (se disponibile):
{pdf_text[:500] if pdf_text else "Non disponibile"}

OUTPUT PROGRAMMA:
{output[:1000]}

TASK:
1. Cerca nel codice le funzionalità richieste dalla TERZA PARTE (es: operatore <<, destructor, copy constructor, operator=)
2. Verifica se sono implementate correttamente
3. Controlla l'output per conferme

RISPONDI IN JSON:
{{
  "implemented": true/false,
  "confidence": 0-100,
  "found_features": ["lista feature trovate"],
  "missing_features": ["lista feature mancanti"],
  "score_suggestion": 0-6,
  "reason": "spiegazione breve"
}}

Analizza SOLO il codice, non l'output atteso mancante."""

            print(f"[AI-TERZA] Analyzing code...")

            response = requests.post(
                f"{self.base_url}/api/generate",
                json={{
                    "model": self.model,
                    "prompt": prompt,
                    "stream": False,
                    "format": "json",
                    "options": {{
                        "temperature": 0.2,
                        "num_predict": 500
                    }}
                }},
                timeout=60
            )

            if response.status_code == 200:
                analysis_text = response.json().get('response', '').strip()
                print(f"[AI-TERZA] Raw response: {analysis_text[:200]}")

                try:
                    analysis = json.loads(analysis_text)
                    print(f"[AI-TERZA] ✅ Parsed: implemented={analysis.get('implemented')}, confidence={analysis.get('confidence')}%")
                    return analysis
                except json.JSONDecodeError:
                    print(f"[AI-TERZA] ⚠️ JSON parse failed")
                    return None
            else:
                print(f"[AI-TERZA] ❌ Request failed: {response.status_code}")
                return None

        except Exception as e:
            print(f"[AI-TERZA] ❌ Error: {e}")
            traceback.print_exc()
            return None

    def analyze_errors(self, risultati, warnings, output, anno, appello):
        if not self.available:
            return None

        try:
            prompt = self._build_prompt(risultati, warnings, output, anno, appello)
            print(f"[AI] Requesting analysis from Ollama...")

            response = requests.post(
                f"{self.base_url}/api/generate",
                json={{
                    "model": self.model,
                    "prompt": prompt,
                    "stream": False,
                    "options": {{
                        "temperature": 0.3,
                        "num_predict": 1000
                    }}
                }},
                timeout=90
            )

            if response.status_code == 200:
                analysis = response.json().get('response', '')
                print(f"[AI] ✅ Analysis received ({len(analysis)} chars)")
                return analysis
            else:
                print(f"[AI] ❌ Request failed: {response.status_code}")
                return None

        except Exception as e:
            print(f"[AI] ❌ Analysis error: {e}")
            return None

    def _build_prompt(self, risultati, warnings, output, anno, appello):
        prompt = f"""Sei un assistente esperto in C++ che analizza test di Fondamenti di Programmazione (UniPi).

TEST: Anno {anno}, Appello {appello}

RISULTATI:
"""
        for parte, info in risultati.items():
            status = info.get('status', 'N/A')
            similarity = info.get('similarity', 0)
            ai_note = info.get('ai_note', '')
            prompt += f"• {parte} PARTE: {status} ({similarity}% similarità)"
            if ai_note:
                prompt += f" - {ai_note}"
            prompt += "\n"

        if warnings:
            prompt += f"\nWARNING ({len(warnings)}): {warnings[0][:150]}\n"

        failed_parts = [p for p, i in risultati.items() if i.get('status') == 'FAIL']
        if failed_parts:
            prompt += f"\nPARTI FALLITE: {', '.join(failed_parts)}\n"

        prompt += """
Fornisci un'analisi in ITALIANO usando MARKDOWN (max 250 parole):

## 🔍 Problemi Identificati
[lista bullet dei problemi]

## 💡 Possibili Cause
[ipotesi sulle cause tecniche]

## ✅ Suggerimenti Pratici
[consigli specifici e actionable]

Usa grassetto **testo** per enfasi, code `snippet` per codice, e tono professionale ma incoraggiante.
"""
        return prompt


class TestRunner:
    BASE_URL = "https://docenti.ing.unipi.it/m.cococcioni/FdP/fondprog_tests"

    def __init__(self, temp_dir):
        self.temp_dir = temp_dir
        self.output_atteso = {}
        self.warning_list = []
        self.asan_errors = []
        self.debug_mode = True
        self.user_code = ""  # NUOVO: salva codice utente

    def normalizza_testo(self, testo):
        if not testo:
            return ""
        lines = [line.rstrip() for line in testo.splitlines()]
        cleaned = []
        prev_empty = False
        for line in lines:
            is_empty = not line.strip()
            if is_empty and prev_empty:
                continue
            cleaned.append(line)
            prev_empty = is_empty
        return "\n".join(cleaned).strip()

    def load_user_code(self):
        """Carica il codice utente per analisi AI"""
        try:
            cpp_files = [f for f in os.listdir(self.temp_dir) if f.endswith('.cpp') and 'main_prof' not in f]
            h_files = [f for f in os.listdir(self.temp_dir) if f.endswith('.h')]

            code_parts = []
            for fname in cpp_files + h_files:
                fpath = os.path.join(self.temp_dir, fname)
                with open(fpath, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    code_parts.append(f"// ===== {fname} =====\n{content}\n")

            self.user_code = "\n".join(code_parts)
            print(f"[CODE] Loaded {len(self.user_code)} chars from user files")
        except Exception as e:
            print(f"[CODE] ⚠️ Could not load user code: {e}")
            self.user_code = ""

    def scarica_pdf(self, anno, appello):
        url = f"{self.BASE_URL}/{anno}_{appello}/testo.pdf"
        try:
            print(f"[DEBUG] Downloading PDF from: {url}")
            r = requests.get(url, timeout=10)
            r.raise_for_status()
            pdf_file = BytesIO(r.content)
            reader = PdfReader(pdf_file)

            testo_completo = ""
            for i, page in enumerate(reader.pages):
                page_text = page.extract_text()
                testo_completo += page_text + "\n\n"
                if self.debug_mode:
                    print(f"[DEBUG] Page {i+1} length: {len(page_text)} chars")

            print(f"[DEBUG] Total PDF text: {len(testo_completo)} chars")

            markers = [
                "USCITA CHE DEVE PRODURRE IL PROGRAMMA",
                "Uscita che deve produrre il programma",
                "OUTPUT ATTESO"
            ]

            output_section = None
            for marker in markers:
                if marker in testo_completo:
                    parts = testo_completo.split(marker, 1)
                    if len(parts) > 1:
                        output_section = parts[1]
                        print(f"[DEBUG] Found marker: {marker}")
                        break

            if not output_section:
                print(f"[WARN] No marker found, using full PDF")
                output_section = testo_completo

            print(f"[DEBUG] Output section (before parse): {len(output_section)} chars")

            self._parse_output_robust(output_section)

            for parte, contenuto in self.output_atteso.items():
                print(f"\n[PDF-{parte}] Extracted: {len(contenuto)} chars")
                if contenuto:
                    preview = contenuto[:200].replace('\n', '↵')
                    print(f"[PDF-{parte}] Preview: {preview}...")
                else:
                    print(f"[PDF-{parte}] ⚠️ EMPTY!")

            return True, None

        except Exception as e:
            error_msg = f"PDF download failed: {str(e)}"
            print(f"[ERROR] {error_msg}")
            traceback.print_exc()
            return False, error_msg

    def _parse_output_robust(self, testo):
        """Parser robusto con regex multipli"""
        self.output_atteso = {"PRIMA": "", "SECONDA": "", "TERZA": ""}

        testo_norm = self.normalizza_testo(testo)

        patterns = {
            'PRIMA': [
                r'[-–—]{2,}\s*PRIMA\s+PARTE\s*[-–—]{2,}',
                r'PRIMA\s+PARTE\s*:?',
                r'\bPRIMA\s+PARTE\b'
            ],
            'SECONDA': [
                r'[-–—]{2,}\s*SECONDA\s+PARTE\s*[-–—]{2,}',
                r'SECONDA\s+PARTE\s*:?',
                r'\bSECONDA\s+PARTE\b'
            ],
            'TERZA': [
                r'[-–—]{2,}\s*TERZA\s+PARTE\s*[-–—]{2,}',
                r'TERZA\s+PARTE\s*:?',
                r'\bTERZA\s+PARTE\b'
            ]
        }

        markers = []
        for parte, pattern_list in patterns.items():
            for pattern in pattern_list:
                for match in re.finditer(pattern, testo_norm, re.IGNORECASE):
                    markers.append({
                        'parte': parte,
                        'start': match.end(),
                        'pos': match.start(),
                        'text': match.group()
                    })
                    print(f"[PARSER] Found '{parte}' at pos {match.start()}")
                    break
            if markers and markers[-1]['parte'] == parte:
                continue

        markers.sort(key=lambda x: x['pos'])

        seen = set()
        unique_markers = []
        for m in markers:
            if m['parte'] not in seen:
                unique_markers.append(m)
                seen.add(m['parte'])
        markers = unique_markers

        print(f"[PARSER] Found {len(markers)} unique markers: {[m['parte'] for m in markers]}")

        for i, marker in enumerate(markers):
            parte = marker['parte']
            start = marker['start']

            if i + 1 < len(markers):
                end = markers[i+1]['pos']
            else:
                remaining = testo_norm[start:]
                cleanup_markers = ["Note per la consegna", "NOTE PER LA CONSEGNA", "Nota per la consegna"]
                end_pos = len(testo_norm)
                for cleanup in cleanup_markers:
                    pos = remaining.find(cleanup)
                    if pos != -1:
                        end_pos = start + pos
                        print(f"[PARSER] Found cleanup marker '{cleanup}' after {parte}")
                        break
                end = end_pos

            content = testo_norm[start:end].strip()
            self.output_atteso[parte] = self.normalizza_testo(content)
            print(f"[PARSER] {parte}: {len(self.output_atteso[parte])} chars extracted")

        if any(len(v) < 10 for v in self.output_atteso.values()):
            print(f"[PARSER] Some parts too short, trying fallback...")
            self._parse_fallback(testo_norm)

    def _parse_fallback(self, testo):
        """Fallback: line-by-line"""
        lines = testo.splitlines()
        current = None
        buffer = []

        for line in lines:
            line_upper = line.strip().upper()

            if any(marker in line_upper for marker in ["NOTE PER LA CONSEGNA", "NOTA PER LA CONSEGNA"]):
                if current and buffer:
                    content = self.normalizza_testo("\n".join(buffer))
                    if len(content) > len(self.output_atteso.get(current, "")):
                        self.output_atteso[current] = content
                break

            if 'PRIMA' in line_upper and 'PARTE' in line_upper:
                if current and buffer:
                    content = self.normalizza_testo("\n".join(buffer))
                    if len(content) > len(self.output_atteso.get(current, "")):
                        self.output_atteso[current] = content
                current = "PRIMA"
                buffer = []
                continue
            elif 'SECONDA' in line_upper and 'PARTE' in line_upper:
                if current and buffer:
                    content = self.normalizza_testo("\n".join(buffer))
                    if len(content) > len(self.output_atteso.get(current, "")):
                        self.output_atteso[current] = content
                current = "SECONDA"
                buffer = []
                continue
            elif 'TERZA' in line_upper and 'PARTE' in line_upper:
                if current and buffer:
                    content = self.normalizza_testo("\n".join(buffer))
                    if len(content) > len(self.output_atteso.get(current, "")):
                        self.output_atteso[current] = content
                current = "TERZA"
                buffer = []
                continue

            if current:
                buffer.append(line)

        if current and buffer:
            content = self.normalizza_testo("\n".join(buffer))
            if len(content) > len(self.output_atteso.get(current, "")):
                self.output_atteso[current] = content

    def scarica_main(self, anno, appello):
        url = f"{self.BASE_URL}/{anno}_{appello}/main.cpp"
        try:
            print(f"[DEBUG] Downloading main.cpp from: {url}")
            r = requests.get(url, timeout=10)
            r.raise_for_status()
            main_path = os.path.join(self.temp_dir, "main_prof.cpp")
            with open(main_path, 'wb') as f:
                f.write(r.content)
            print(f"[DEBUG] main.cpp downloaded")
            return True, None
        except Exception as e:
            error_msg = f"main.cpp download failed: {str(e)}"
            print(f"[ERROR] {error_msg}")
            return False, error_msg

    def _parse_asan_errors(self, stderr):
        """Parser per errori AddressSanitizer"""
        asan_errors = []

        asan_patterns = {
            'heap-buffer-overflow': '❌ Heap Buffer Overflow',
            'stack-buffer-overflow': '❌ Stack Buffer Overflow',
            'use-after-free': '❌ Use After Free',
            'double-free': '❌ Double Free',
            'memory leak': '⚠️ Memory Leak',
            'heap-use-after-free': '❌ Heap Use After Free',
            'global-buffer-overflow': '❌ Global Buffer Overflow',
            'stack-use-after-return': '❌ Stack Use After Return',
            'alloc-dealloc-mismatch': '❌ Alloc/Dealloc Mismatch'
        }

        lines = stderr.splitlines()
        current_error = None
        stack_trace = []

        for i, line in enumerate(lines):
            for pattern, label in asan_patterns.items():
                if pattern in line.lower():
                    if current_error:
                        current_error['stack_trace'] = '\n'.join(stack_trace[:10])
                        asan_errors.append(current_error)

                    current_error = {
                        'type': label,
                        'message': line.strip(),
                        'stack_trace': ''
                    }
                    stack_trace = []
                    break

            if current_error and ('#' in line or 'at ' in line.lower() or line.strip().startswith('0x')):
                # Evidenzia linee con codice utente
                if '/compito.' in line or '.cpp:' in line or '.h:' in line:
                    stack_trace.append('⚠️ ' + line.strip())
                else:
                    stack_trace.append(line.strip())

        if current_error:
            current_error['stack_trace'] = '\n'.join(stack_trace[:10])
            asan_errors.append(current_error)

        return asan_errors

    def compila_ed_esegui(self, usa_asan=True, timeout=15):
        try:
            print(f"[DEBUG] Compiling...")
            cpp_files = [f for f in os.listdir(self.temp_dir) if f.endswith('.cpp')]
            print(f"[DEBUG] Files: {cpp_files}")

            if not cpp_files:
                return None, "No .cpp files", "ERRORE_COMPILAZIONE"

            exe_path = os.path.join(self.temp_dir, "test.exe")
            files_path = [os.path.join(self.temp_dir, f) for f in cpp_files]

            cmd = ["g++", "-Wall", "-o", exe_path] + files_path
            if usa_asan:
                cmd += ["-g", "-fsanitize=address"]

            res = subprocess.run(cmd, capture_output=True, text=True, cwd=self.temp_dir)
            exe_creato = os.path.exists(exe_path)

            if not exe_creato and usa_asan:
                print(f"[WARN] Retrying without ASan...")
                cmd = ["g++", "-Wall", "-o", exe_path] + files_path
                res = subprocess.run(cmd, capture_output=True, text=True, cwd=self.temp_dir)
                exe_creato = os.path.exists(exe_path)
                usa_asan = False

            if not exe_creato:
                return None, res.stderr, "ERRORE_COMPILAZIONE"

            self.warning_list = [l for l in res.stderr.splitlines() if "warning:" in l.lower()]
            print(f"[DEBUG] Compilation OK. Warnings: {len(self.warning_list)}")

            print(f"[DEBUG] Executing...")
            try:
                exe_res = subprocess.run([exe_path], capture_output=True, text=True, timeout=timeout)
                print(f"[DEBUG] Execution OK. Return: {exe_res.returncode}")

                if usa_asan and exe_res.stderr:
                    self.asan_errors = self._parse_asan_errors(exe_res.stderr)
                    if self.asan_errors:
                        print(f"[ASAN] Found {len(self.asan_errors)} memory errors!")
                        for err in self.asan_errors:
                            print(f"[ASAN] - {err['type']}")

                return exe_res.stdout, exe_res.stderr, "OK"
            except subprocess.TimeoutExpired:
                return None, "TIMEOUT", "TIMEOUT"

        except Exception as e:
            print(f"[ERROR] {str(e)}")
            traceback.print_exc()
            return None, str(e), "ERRORE"

    def valuta_output(self, output, analyzer=None):
        try:
            print(f"\n[EVAL] ==========================================")
            punteggio = 0
            risultati_parti = {}
            output_normalizzato = self.normalizza_testo(output)

            for parte in ["PRIMA", "SECONDA", "TERZA"]:
                print(f"\n[EVAL-{parte}] Evaluating...")

                atteso = self.normalizza_testo(self.output_atteso.get(parte, ""))

                if not atteso or len(atteso) < 10:
                    print(f"[EVAL-{parte}] ⚠️  Expected too short: {len(atteso)} chars")

                    # NUOVO: Per TERZA PARTE, prova AI code analysis
                    if parte == "TERZA" and analyzer and analyzer.available:
                        print(f"[EVAL-TERZA] 🤖 Trying AI code analysis...")
                        ai_result = analyzer.analyze_terza_parte(
                            self.user_code,
                            atteso,
                            output_normalizzato
                        )

                        if ai_result and ai_result.get('implemented'):
                            confidence = ai_result.get('confidence', 0)
                            score_sugg = ai_result.get('score_suggestion', 0)
                            reason = ai_result.get('reason', 'AI verified implementation')

                            print(f"[EVAL-TERZA] 🤖 AI says: IMPLEMENTED (confidence: {confidence}%)")
                            print(f"[EVAL-TERZA] 🤖 Suggested score: {score_sugg}/6")

                            if confidence >= 70:
                                punteggio += score_sugg
                                risultati_parti[parte] = {
                                    "status": "OK" if score_sugg >= 5 else "PARTIAL",
                                    "similarity": confidence,
                                    "expected": "Parser failed, AI verified",
                                    "obtained": f"AI: {score_sugg}/6 punti",
                                    "ai_verified": True,
                                    "ai_note": reason
                                }
                                print(f"[EVAL-TERZA] ✅ AI APPROVED: +{score_sugg} punti")
                                continue

                    risultati_parti[parte] = {
                        "status": "N/A",
                        "similarity": 0,
                        "expected": "Parser failed",
                        "obtained": "N/A"
                    }
                    continue

                ottenuto = self._estrai_sezione_migliorata(output_normalizzato, parte)

                print(f"[EVAL-{parte}] Expected: {len(atteso)} | Obtained: {len(ottenuto)}")

                if self.debug_mode:
                    print(f"[EVAL-{parte}] Expected preview: {atteso[:100].replace(chr(10), '↵')}")
                    print(f"[EVAL-{parte}] Obtained preview: {ottenuto[:100].replace(chr(10), '↵')}")

                ratio1 = difflib.SequenceMatcher(None, atteso, ottenuto).ratio()
                atteso_flex = re.sub(r'\s+', ' ', atteso).strip()
                ottenuto_flex = re.sub(r'\s+', ' ', ottenuto).strip()
                ratio2 = difflib.SequenceMatcher(None, atteso_flex, ottenuto_flex).ratio()
                atteso_lines = [l.strip() for l in atteso.splitlines() if l.strip()]
                ottenuto_lines = [l.strip() for l in ottenuto.splitlines() if l.strip()]
                ratio3 = difflib.SequenceMatcher(None, atteso_lines, ottenuto_lines).ratio()

                ratio = max(ratio1, ratio2, ratio3)
                print(f"[EVAL-{parte}] Ratios: {ratio1:.2%} | {ratio2:.2%} | {ratio3:.2%} → Best: {ratio:.2%}")

                passed = ratio >= 0.80
                print(f"[EVAL-{parte}] {'✅ PASSED' if passed else '❌ FAILED'} (threshold: 80%)")

                risultati_parti[parte] = {
                    "status": "OK" if passed else "FAIL",
                    "similarity": round(ratio * 100, 1),
                    "expected": atteso[:300],
                    "obtained": ottenuto[:300]
                }

                if passed:
                    punteggio += {"PRIMA": 18, "SECONDA": 6, "TERZA": 6}.get(parte, 0)

            n_warn = len(self.warning_list)
            if n_warn > 0:
                malus = min(2, n_warn)
                punteggio -= malus
                print(f"\n[EVAL] Warning penalty: -{malus}")

            if self.asan_errors:
                print(f"\n[EVAL] ASan errors: {len(self.asan_errors)} (shown but not penalized)")

            print(f"\n[EVAL] Final score: {punteggio}/30")
            print(f"[EVAL] ==========================================\n")

            return punteggio, risultati_parti
        except Exception as e:
            print(f"[ERROR] Evaluation error: {str(e)}")
            traceback.print_exc()
            return 0, {}

    def _estrai_sezione_migliorata(self, output, parte):
        """Estrazione migliorata con pattern multipli"""
        lines = output.splitlines()

        patterns = {
            "PRIMA": [
                r'[-–—]{2,}\s*PRIMA\s+PARTE\s*[-–—]{2,}',
                r'PRIMA\s+PARTE',
                r'\bPRIMA\b'
            ],
            "SECONDA": [
                r'[-–—]{2,}\s*SECONDA\s+PARTE\s*[-–—]{2,}',
                r'SECONDA\s+PARTE',
                r'\bSECONDA\b'
            ],
            "TERZA": [
                r'[-–—]{2,}\s*TERZA\s+PARTE\s*[-–—]{2,}',
                r'TERZA\s+PARTE',
                r'\bTERZA\b'
            ]
        }

        all_patterns = []
        for p, pats in patterns.items():
            if p != parte:
                all_patterns.extend(pats)

        start_idx = -1
        for i, line in enumerate(lines):
            for pattern in patterns[parte]:
                if re.search(pattern, line, re.IGNORECASE):
                    start_idx = i + 1
                    print(f"[EXTRACT-{parte}] Found start at line {i}")
                    break
            if start_idx != -1:
                break

        if start_idx == -1:
            print(f"[EXTRACT-{parte}] ⚠️  Marker not found in output")
            return ""

        end_idx = len(lines)
        for i in range(start_idx, len(lines)):
            for pattern in all_patterns:
                if re.search(pattern, lines[i], re.IGNORECASE):
                    end_idx = i
                    print(f"[EXTRACT-{parte}] Found end at line {i}")
                    break
            if end_idx != len(lines):
                break

        content = "\n".join(lines[start_idx:end_idx])
        return self.normalizza_testo(content)


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/test', methods=['POST'])
def run_test():
    print("\n" + "="*70)
    print("[API] ========== NEW TEST REQUEST ==========")
    print("="*70)

    temp_dir = None
    try:
        anno = request.form.get('anno')
        appello = request.form.get('appello')
        files = request.files.getlist('files')

        print(f"[API] Parameters: anno={anno}, appello={appello}, files={len(files)}")

        if not files:
            return jsonify({'error': 'No files uploaded'}), 400
        if not anno or not appello:
            return jsonify({'error': 'Anno and appello required'}), 400

        temp_dir = tempfile.mkdtemp()

        saved_files = []
        for file in files:
            if file.filename and file.filename.endswith(('.cpp', '.h')):
                filename = secure_filename(file.filename)
                filepath = os.path.join(temp_dir, filename)
                file.save(filepath)
                saved_files.append(filename)

        if not saved_files:
            return jsonify({'error': 'No valid files'}), 400

        runner = TestRunner(temp_dir)

        print("\n[STEP 1] PDF Download")
        success, error = runner.scarica_pdf(anno, appello)
        if not success:
            return jsonify({'error': f'PDF failed: {error}'}), 400

        print("\n[STEP 2] Main Download")
        success, error = runner.scarica_main(anno, appello)
        if not success:
            return jsonify({'error': f'Main failed: {error}'}), 400

        print("\n[STEP 3] Load User Code")
        runner.load_user_code()

        print("\n[STEP 4] Compile & Execute")
        output, stderr, stato = runner.compila_ed_esegui()

        if stato == "ERRORE_COMPILAZIONE":
            return jsonify({'error': 'Compilation error', 'details': stderr[:1000]}), 400
        if stato == "TIMEOUT":
            return jsonify({'error': 'Execution timeout'}), 400
        if stato == "ERRORE":
            return jsonify({'error': 'Execution error', 'details': stderr[:1000]}), 400

        print("\n[STEP 5] Evaluation (with AI if needed)")
        analyzer = OllamaAnalyzer()
        voto, risultati_parti = runner.valuta_output(output, analyzer)

        test_id = f"{anno}_{appello}_{datetime.now().strftime('%Y%m%d%H%M%S')}"
        test_cache[test_id] = {
            'risultati': risultati_parti,
            'warnings': runner.warning_list,
            'asan_errors': runner.asan_errors,
            'output': output,
            'anno': anno,
            'appello': appello,
            'voto': voto
        }

        result = {
            'test_id': test_id,
            'voto': voto,
            'voto_text': '30 e LODE' if voto > 30 else f'{voto}/30',
            'passed': voto >= 18,
            'parti': risultati_parti,
            'warning': runner.warning_list,
            'asan_errors': runner.asan_errors,
            'output': output,
            'ollama_available': analyzer.available,
            'needs_ai': voto < 30 and analyzer.available,
            'timestamp': datetime.now().isoformat()
        }

        test_history.append({
            'anno': anno,
            'appello': appello,
            'voto': voto,
            'timestamp': result['timestamp']
        })

        print(f"\n[API] TEST COMPLETED: {voto}/30")
        print("="*70 + "\n")

        return jsonify(result)

    except Exception as e:
        error_msg = f"Error: {str(e)}"
        print(f"\n[ERROR] {error_msg}")
        traceback.print_exc()
        return jsonify({'error': error_msg}), 500

    finally:
        if temp_dir and os.path.exists(temp_dir):
            try:
                shutil.rmtree(temp_dir)
            except:
                pass

@app.route('/api/ai-analysis/<test_id>', methods=['GET'])
def get_ai_analysis(test_id):
    print(f"\n[AI-API] Analysis for: {test_id}")

    if test_id not in test_cache:
        return jsonify({'error': 'Test not found'}), 404

    test_data = test_cache[test_id]

    if test_data['voto'] >= 30:
        return jsonify({'success': False, 'message': 'Test perfetto!'})

    analyzer = OllamaAnalyzer()
    if not analyzer.available:
        return jsonify({'success': False, 'message': 'Ollama non disponibile'})

    try:
        analysis = analyzer.analyze_errors(
            test_data['risultati'],
            test_data['warnings'],
            test_data['output'],
            test_data['anno'],
            test_data['appello']
        )

        if analysis:
            return jsonify({'success': True, 'analysis': analysis})
        else:
            return jsonify({'success': False, 'message': 'Analisi fallita'})
    except Exception as e:
        return jsonify({'success': False, 'message': f'Errore: {str(e)}'}), 500

@app.route('/api/history')
def get_history():
    return jsonify(test_history[-10:])

@app.route('/api/health')
def health_check():
    analyzer = OllamaAnalyzer()
    return jsonify({
        'status': 'ok',
        'timestamp': datetime.now().isoformat(),
        'tests_count': len(test_history),
        'ollama_available': analyzer.available
    })

if __name__ == '__main__':
    print("\n" + "="*70)
    print("🚀 FdP AutoCorrezione Server - v7.2 (AI Code Analysis)")
    print("="*70)
    print("📍 Server: http://localhost:5000")
    print("✨ Features:")
    print("   • Ultra-robust PDF parser")
    print("   • 🤖 AI code analysis for TERZA PARTE")
    print("   • Improved section extraction (80% threshold)")
    print("   • Dark/Light mode + Markdown AI")
    print("   • 🛡️  ASan memory error detection")

    analyzer = OllamaAnalyzer()
    if analyzer.available:
        print(f"   • 🤖 AI: ENABLED (code verification active)")
    else:
        print(f"   • 🤖 AI: DISABLED")

    print("="*70 + "\n")
    app.run(debug=True, port=5000, host='0.0.0.0')
