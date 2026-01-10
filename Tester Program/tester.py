import os
import subprocess
import requests
import sys
import difflib
import re
import random
import tkinter as tk
from tkinter import filedialog
from pypdf import PdfReader
from io import BytesIO

# --- CONFIGURAZIONE ---
BASE_URL = "https://docenti.ing.unipi.it/m.cococcioni/FdP/fondprog_tests"
NOME_MAIN_PROF = "main_test_prof.cpp"
NOME_MAIN_RIDOTTO = "main_test_prof_no_part3.cpp"
NOME_FUZZER = "fuzz_test_auto.cpp"
NOME_ESEGUIBILE = "test_esame"
TIMEOUT_ESECUZIONE = 15

# Gestione colori
try:
    from colorama import init, Fore, Style
    init(autoreset=True)
except ImportError:
    class Fore: RED = ""; GREEN = ""; YELLOW = ""; CYAN = ""; RESET = ""
    class Style: BRIGHT = ""; RESET_ALL = ""; DIM = ""

def log_info(msg): print(f"{Fore.CYAN}ℹ️  {msg}{Fore.RESET}")
def log_success(msg): print(f"{Fore.GREEN}✅ {msg}{Fore.RESET}")
def log_error(msg): print(f"{Fore.RED}❌ {msg}{Fore.RESET}")
def log_warn(msg): print(f"{Fore.YELLOW}⚠️ {msg}{Fore.RESET}")

class InterpreteErrori:
    @staticmethod
    def analizza_warning(raw_warning):
        msg = "Warning generico."
        suggerimento = "Controlla la riga indicata."
        
        if "sign-compare" in raw_warning:
            msg = "Confronto tra tipi con segno diverso (int vs unsigned)."
            suggerimento = "Nei cicli for, usa 'unsigned int i' invece di 'int i'."
        elif "unused variable" in raw_warning:
            msg = "Variabile non utilizzata."
            suggerimento = "Rimuovila o usala."
        elif "return-type" in raw_warning:
            msg = "Manca il return in una funzione non void."
            suggerimento = "La funzione deve ritornare un valore in tutti i casi."
        elif "write-strings" in raw_warning:
            msg = "Conversione stringa costante -> char*."
            suggerimento = "Aggiungi 'const' ai parametri (es. 'const char nome[]')."
        
        match = re.search(r'([a-zA-Z0-9_]+\.cpp):(\d+):', raw_warning)
        loc = f"{match.group(1)} riga {match.group(2)}" if match else "..."
        return f"{Fore.YELLOW}[{loc}]{Fore.RESET} {msg}\n    💡 {Style.DIM}{suggerimento}{Style.RESET_ALL}"

class FuzzGenerator:
    def __init__(self, cartella):
        self.cartella = cartella
        self.header_file = os.path.join(cartella, "compito.h")
        self.class_name = ""
        self.methods = []
        self.constructor_args = []

    def analizza_header(self):
        if not os.path.exists(self.header_file): return False
        with open(self.header_file, 'r', encoding='utf-8', errors='ignore') as f: content = f.read()
        
        match_class = re.search(r'class\s+(\w+)', content)
        if match_class: self.class_name = match_class.group(1)
        else: return False

        lines = content.splitlines()
        is_public = False
        for line in lines:
            line = line.strip()
            if "public:" in line: is_public = True; continue
            if "private:" in line: is_public = False; continue
            
            if is_public and "(" in line and ");" in line:
                if "~" in line or "operator" in line or "friend" in line: continue
                if self.class_name in line and "(" in line:
                    args = self._parse_args(line)
                    if not self.constructor_args: self.constructor_args = args
                    continue
                method_name = re.search(r'\s+(\w+)\s*\(', line)
                if method_name:
                    name = method_name.group(1)
                    if name == self.class_name: continue
                    args = self._parse_args(line)
                    self.methods.append((name, args))
        return True

    def _parse_args(self, line):
        match = re.search(r'\((.*)\)', line)
        if not match: return []
        raw_args = match.group(1).split(',')
        parsed = []
        for arg in raw_args:
            arg = arg.strip()
            if not arg: continue
            tokens = arg.split()
            tipo = "int" 
            for t in tokens:
                if t in ["int", "double", "float", "bool", "char", "long", "short", "unsigned"]:
                    tipo = t
                    if "*" in arg or "[" in arg: tipo = "char*" if "char" in t else "int*"
                    break
            if "char" in arg and ("*" in arg or "[" in arg): tipo = "char*"
            parsed.append(tipo)
        return parsed

    def genera_codice_fuzzer(self):
        if not self.class_name: return None
        cpp_code = f"""
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include "compito.h"
using namespace std;
int rand_int(int min, int max) {{ return min + rand() % (max - min + 1); }}
bool rand_bool() {{ return rand() % 2 == 0; }}
void rand_str(char* dest, int len) {{
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz ";
    for (int i = 0; i < len; ++i) dest[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    dest[len] = 0;
}}
int main() {{
    srand(time(0));
    cout << "--- INIZIO STRESS TEST AUTOMATICO ---" << endl;
    """
        args_str = []
        for arg_type in self.constructor_args:
            if arg_type == "int": args_str.append(str(random.randint(1, 10)))
            elif arg_type == "char*": args_str.append('"TestName"')
            else: args_str.append("0")
        
        costruttore_call = f"{self.class_name} obj({', '.join(args_str)});" if args_str else f"{self.class_name} obj;"
        cpp_code += f"    {costruttore_call}\n\n"
        cpp_code += "    cout << \"2. Esecuzione metodi random (100 iterazioni)...\" << endl;\n"
        cpp_code += "    char buffer[50];\n"
        cpp_code += "    for(int i=0; i<100; i++) {\n"
        
        if self.methods:
            cpp_code += "        int choice = rand() % " + str(len(self.methods)) + ";\n"
            for idx, (name, args) in enumerate(self.methods):
                if idx == 0: cpp_code += f"        if (choice == {idx}) {{\n"
                else: cpp_code += f"        }} else if (choice == {idx}) {{\n"
                call_args = []
                for at in args:
                    if "char*" in at: call_args.append("buffer")
                    elif "bool" in at: call_args.append("rand_bool()")
                    else: call_args.append("rand_int(0, 100)")
                if any("char*" in a for a in args):
                    cpp_code += "            rand_str(buffer, rand_int(0, 30));\n"
                cpp_code += f"            cout << \"[CALL] {name} \" << i << endl;\n" # LOG DI DEBUG
                cpp_code += f"            obj.{name}({', '.join(call_args)});\n"
            cpp_code += "        }\n"

        cpp_code += """
    }
    cout << "--- STRESS TEST COMPLETATO SENZA CRASH ---" << endl;
    return 0;
}
"""
        return cpp_code


class EsameTester:
    def __init__(self):
        self.cartella = ""
        self.anno = ""
        self.appello = ""
        self.output_atteso_completo = ""
        self.sezioni_attese = {"PRIMA": "", "SECONDA": "", "TERZA": ""}
        self.warning_list = []
    
    def seleziona_cartella(self):
        root = tk.Tk()
        root.withdraw()
        root.attributes('-topmost', True)
        root.lift()
        root.focus_force()
        self.cartella = filedialog.askdirectory(title="Seleziona la cartella con i tuoi file .cpp", parent=root)
        root.destroy()
        return bool(self.cartella)

    def normalizza_testo(self, testo):
        if not testo: return ""
        lines = testo.splitlines()
        cleaned = [line.rstrip() for line in lines]
        return "\n".join(cleaned).strip()

    def estrai_sezione_utente(self, output_completo, nome_sezione):
        lines = output_completo.splitlines()
        buffer = []
        cattura = False
        header_map = {"PRIMA": "--- PRIMA PARTE ---", "SECONDA": "--- SECONDA PARTE ---", "TERZA": "--- TERZA PARTE ---"}
        target = header_map.get(nome_sezione)
        next_headers = [h for h in header_map.values() if h != target]
        for line in lines:
            if target and target in line: cattura = True; continue
            if any(nh in line for nh in next_headers): cattura = False
            if cattura: buffer.append(line)
        return "\n".join(buffer).strip()

    def scarica_output_atteso(self):
        url = f"{BASE_URL}/{self.anno}_{self.appello}/testo.pdf"
        log_info(f"Scarico PDF da: {url} ...")
        try:
            r = requests.get(url)
            if r.status_code == 404:
                log_error("PDF non trovato."); return False
            pdf_file = BytesIO(r.content)
            reader = PdfReader(pdf_file)
            testo_pag = reader.pages[-1].extract_text()
            m_start = "Uscita che deve produrre il programma"
            if m_start in testo_pag:
                p_utile = testo_pag.split(m_start)[1]
                if "Note per la consegna" in p_utile: p_utile = p_utile.split("Note per la consegna")[0]
                self.output_atteso_completo = p_utile
                self._parse_sezioni_output(self.output_atteso_completo)
                return True
            else:
                self.output_atteso_completo = testo_pag
                self._parse_sezioni_output(testo_pag)
                return True
        except: return False

    def _parse_sezioni_output(self, testo):
        self.sezioni_attese = {"PRIMA": "", "SECONDA": "", "TERZA": ""}
        lines = testo.splitlines()
        current = "PRIMA"
        buffer = []
        for line in lines:
            lup = line.upper()
            if "PRIMA PARTE" in lup:
                if buffer: self.sezioni_attese[current] = "\n".join(buffer)
                current = "PRIMA"; buffer = []
            elif "SECONDA PARTE" in lup:
                if buffer: self.sezioni_attese[current] = "\n".join(buffer)
                current = "SECONDA"; buffer = []
            elif "TERZA PARTE" in lup:
                if buffer: self.sezioni_attese[current] = "\n".join(buffer)
                current = "TERZA"; buffer = []
            else: buffer.append(line)
        if buffer: self.sezioni_attese[current] = "\n".join(buffer)

    def scarica_main_prof(self):
        url = f"{BASE_URL}/{self.anno}_{self.appello}/main.cpp"
        dest = os.path.join(self.cartella, NOME_MAIN_PROF)
        try:
            r = requests.get(url)
            if r.status_code == 200:
                with open(dest, 'wb') as f: f.write(r.content)
                return True
        except: pass
        return False

    def crea_main_senza_terza_parte(self):
        path_full = os.path.join(self.cartella, NOME_MAIN_PROF)
        path_reduced = os.path.join(self.cartella, NOME_MAIN_RIDOTTO)
        if not os.path.exists(path_full): return False
        with open(path_full, 'r', encoding='utf-8', errors='ignore') as f: content = f.read()
        idx = content.find("TERZA PARTE")
        if idx >= 0:
            cut = content.rfind('\n', 0, idx)
            if cut == -1: cut = idx
            last_brace = content.rfind("}")
            if last_brace > cut:
                new_c = content[:cut] + "\n    // TERZA PARTE RIMOSSA\n    return 0;\n" + content[last_brace:]
                with open(path_reduced, 'w', encoding='utf-8') as f: f.write(new_c)
                return True
        return False

    def compila(self, main_cpp, usa_asan=True, output_exe=NOME_ESEGUIBILE):
        exclude_files = [NOME_MAIN_PROF, NOME_MAIN_RIDOTTO, NOME_FUZZER, "main.cpp"]
        files = [f for f in os.listdir(self.cartella) 
                 if f.endswith('.cpp') and f not in exclude_files and "test_prof" not in f]
        files.append(main_cpp)
        
        exe_path = os.path.join(self.cartella, f"{output_exe}.exe")
        files_path = [os.path.join(self.cartella, f) for f in files]
        
        cmd = ["g++", "-Wall", "-o", exe_path] + files_path
        if usa_asan: cmd += ["-g", "-O1", "-fsanitize=address", "-fno-omit-frame-pointer"]
        
        res = subprocess.run(cmd, capture_output=True, text=True)
        
        if res.returncode == 0 and main_cpp != NOME_FUZZER:
            self.warning_list = []
            for line in res.stderr.splitlines():
                if "warning:" in line:
                    self.warning_list.append(line)
        
        return exe_path, res.returncode, res.stderr

    def esegui_fuzzer(self):
        fuzzer = FuzzGenerator(self.cartella)
        if not fuzzer.analizza_header():
            log_warn("Impossibile analizzare compito.h per generare test automatici.")
            return True # Skip
        
        codice = fuzzer.genera_codice_fuzzer()
        if not codice: return True
        
        path_fuzzer = os.path.join(self.cartella, NOME_FUZZER)
        with open(path_fuzzer, 'w') as f: f.write(codice)
        
        log_info("Compilazione ed esecuzione STRESS TEST (Fuzzer)...")
        
        exe, ret, err = self.compila(NOME_FUZZER, usa_asan=True, output_exe="stress_test")
        
        asan_active = True
        if ret != 0 and ("cannot find -lasan" in err or "ld returned 1" in err):
             # Riprova senza ASan
             asan_active = False
             exe, ret, err = self.compila(NOME_FUZZER, usa_asan=False, output_exe="stress_test")
        
        if ret != 0:
            log_warn(f"Il Fuzzer non compila.\n{err}")
            return True 
            
        try:
            res = subprocess.run([exe], capture_output=True, text=True, timeout=10)
            if res.returncode != 0:
                print(f"\n{Fore.RED}💥 CRASH RILEVATO NEL FUZZER! (Exit Code: {res.returncode}){Fore.RESET}")
                
                # --- NOVITÀ: Stampa il log completo ---
                print(f"{Style.BRIGHT}--- LOG STANDARD OUTPUT (Cosa ha stampato prima di morire) ---{Style.RESET_ALL}")
                print(res.stdout if res.stdout else "(Nessun output su stdout)")
                
                print(f"{Style.BRIGHT}--- LOG ERROR OUTPUT (Messaggi di errore/ASan) ---{Style.RESET_ALL}")
                print(res.stderr if res.stderr else "(Nessun errore su stderr - Crash silenzioso?)")
                
                if not res.stderr and not asan_active:
                    print(f"\n{Fore.YELLOW}⚠️  NOTA: Il crash è stato silenzioso perché AddressSanitizer non è attivo.{Fore.RESET}")
                    print(f"   Il file '{NOME_FUZZER}' NON verrà cancellato.")
                    print(f"   Aprilo e prova a compilarlo manualmente per debuggare.")
                    return False # Return false indica fallimento, ma lasciamo il file
                
                # Se c'è stato crash, NON CANCELLIAMO IL FILE FUZZER per permettere debug
                return False 
            else:
                print(f"{Fore.GREEN}🛡️  Il codice ha superato lo Stress Test.{Fore.RESET}")
                # Se passa, possiamo cancellarlo
                if os.path.exists(path_fuzzer): os.remove(path_fuzzer)
                return True
        except subprocess.TimeoutExpired:
            print(f"{Fore.RED}⏳ Timeout durante lo Stress Test.{Fore.RESET}")
            return False

    def esegui_test_principale(self):
        log_info("Compilazione Test Professore...")
        exe, ret, err = self.compila(NOME_MAIN_PROF, usa_asan=True)
        modo = "COMPLETO"
        
        if ret != 0:
            if "cannot find -lasan" in err or "ld returned 1" in err:
                exe, ret, err = self.compila(NOME_MAIN_PROF, usa_asan=False)
            
            if ret != 0:
                log_warn("Compilazione Completa fallita. Provo Ridotta...")
                if self.crea_main_senza_terza_parte():
                    exe, ret, err = self.compila(NOME_MAIN_RIDOTTO, usa_asan=False)
                    modo = "RIDOTTO"
                if ret != 0:
                    log_error(f"Errore Compilazione:\n{err}")
                    return None, None, "FALLITO"
        
        env = os.environ.copy(); env['ASAN_OPTIONS'] = "color=always"
        try:
            res = subprocess.run([exe], capture_output=True, text=True, timeout=TIMEOUT_ESECUZIONE, env=env)
            return res.stdout, res.stderr, modo
        except: return None, "TIMEOUT", modo

    def calcola_voto_sfumato(self, output, modo, passed_fuzzer):
        punteggio = 0
        output_clean = self.normalizza_testo(output)
        
        print(f"\n{Style.BRIGHT}--- VALUTAZIONE DETTAGLIATA ---{Style.RESET_ALL}")
        
        # 1. Base Punteggio
        p1 = self._check_section("PRIMA", output_clean)
        p2 = self._check_section("SECONDA", output_clean)
        p3 = False
        
        if modo == "RIDOTTO": pass
        else:
            atteso_3 = self.sezioni_attese["TERZA"].strip()
            if not atteso_3: p3 = True
            else: p3 = self._check_section("TERZA", output_clean)

        if p1: punteggio += 18; log_success("PRIMA PARTE: OK (+18)")
        else: log_error("PRIMA PARTE: Errori (+0)")
        
        if p2: punteggio += 6; log_success("SECONDA PARTE: OK (+6)")
        else: log_warn("SECONDA PARTE: Errori (+0)")
        
        if p3: punteggio += 6; log_success("TERZA PARTE: OK (+6)")
        else: log_warn("TERZA PARTE: Mancante o Errori (+0)")

        # 2. Modificatori
        print(f"\n{Style.DIM}--- Analisi Qualità ---{Style.RESET_ALL}")
        
        # Warning
        n_warn = len(self.warning_list)
        if n_warn > 0:
            malus = min(2, n_warn)
            punteggio -= malus
            print(f"{Fore.YELLOW}⚠️  Ci sono {n_warn} warning (-{malus}){Fore.RESET}")
            for w in self.warning_list[:3]:
                print(InterpreteErrori.analizza_warning(w))
        else:
            print(f"{Fore.GREEN}✨ Codice pulito (0 warning){Fore.RESET}")

        # Fuzzer
        if not passed_fuzzer:
            punteggio -= 4
            print(f"{Fore.RED}💥 Penalità grave: Il codice crasha sotto stress (-4){Fore.RESET}")
            print("    (Controlla l'output del Fuzzer sopra per i dettagli)")
        else:
            punteggio += 3
            print(f"{Fore.GREEN}🛡️  Bonus Stabilità: Il codice è robusto (+3){Fore.RESET}")

        # BONUS LODE
        if n_warn == 0 and passed_fuzzer and punteggio == 30:
            punteggio += 2
            print(f"{Fore.CYAN}💎 BONUS ECCELLENZA: Codice perfetto e robusto (+2){Fore.RESET}")

        if punteggio > 30:
            print(f"\n{Style.BRIGHT}VOTO FINALE: {Fore.MAGENTA}30 e LODE 🏆{Fore.RESET}{Style.RESET_ALL}\n")
        else:
            punteggio = max(0, min(30, punteggio))
            colore = Fore.GREEN if punteggio >= 18 else Fore.RED
            print(f"\n{Style.BRIGHT}VOTO FINALE: {colore}{punteggio}/30{Fore.RESET}{Style.RESET_ALL}\n")

    def _check_section(self, nome, out_full):
        atteso = self.normalizza_testo(self.sezioni_attese.get(nome, ""))
        if not atteso: return True
        out_sez = self.normalizza_testo(self.estrai_sezione_utente(out_full, nome))
        m = difflib.SequenceMatcher(None, atteso, out_sez)
        return m.find_longest_match(0, len(atteso), 0, len(out_sez)).size >= len(atteso) * 0.95

    def run(self):
        print(f"{Style.BRIGHT}--- SUPER TESTER AI (Debug Edition) ---{Style.RESET_ALL}\n")
        self.anno = input("Anno: ").strip()
        self.appello = input("Appello: ").strip()
        
        if not self.seleziona_cartella(): return
        if not self.scarica_output_atteso(): return
        if not self.scarica_main_prof(): return

        passed_fuzzer = self.esegui_fuzzer()
        stdout, stderr, modo = self.esegui_test_principale()
        
        if stdout is None: return

        if stderr and "ERROR: AddressSanitizer" in stderr:
            print(f"\n{Fore.RED}!!! ERRORE DI MEMORIA NEL MAIN !!!{Fore.RESET}")
        
        print(f"\n{Style.DIM}--- Output Main ---{Style.RESET_ALL}")
        print(stdout)

        self.calcola_voto_sfumato(stdout, modo, passed_fuzzer)
        
        # Pulizia (mantiene il fuzzer se crasha)
        files_to_clean = [NOME_MAIN_PROF, NOME_MAIN_RIDOTTO, f"{NOME_ESEGUIBILE}.exe", "stress_test.exe"]
        for f in files_to_clean:
            p = os.path.join(self.cartella, f)
            if os.path.exists(p): os.remove(p)

if __name__ == "__main__":
    tester = EsameTester()
    tester.run()