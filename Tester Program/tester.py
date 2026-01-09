import os
import subprocess
import requests
import sys
import tkinter as tk
from tkinter import filedialog
from pypdf import PdfReader
from io import BytesIO

# --- CONFIGURAZIONE ---
BASE_URL = "https://docenti.ing.unipi.it/m.cococcioni/FdP/fondprog_tests"
NOME_MAIN_PROF = "main_test_prof_temp.cpp"
NOME_ESEGUIBILE = "test_esame.exe"
# ----------------------

def seleziona_cartella():
    """Apre finestra per scegliere la cartella"""
    root = tk.Tk()
    root.withdraw()
    folder_selected = filedialog.askdirectory(title="Seleziona la cartella con i tuoi file .cpp")
    return folder_selected

def scarica_e_estrai_output(anno, appello):
    """
    Scarica il PDF e cerca l'output tra i marcatori specifici
    visti nel file testo.pdf (Pagina 3).
    """
    # Costruiamo l'URL. Nota: se l'utente scrive '2', l'URL userà '2'.
    url = f"{BASE_URL}/{anno}_{appello}/testo.pdf"
    print(f"⬇️  Scarico PDF da: {url} ...")
    
    try:
        response = requests.get(url)
        if response.status_code == 404:
            print("❌ PDF non trovato (Errore 404). Verifica Anno e Appello.")
            return None
        response.raise_for_status()
        
        pdf_file = BytesIO(response.content)
        reader = PdfReader(pdf_file)
        
        # L'output è quasi sempre nell'ultima pagina [cite: 60]
        testo_pag = reader.pages[-1].extract_text()
        
        # --- LOGICA DI ESTRAZIONE BASATA SUL TUO PDF ---
        marcatore_inizio = "Uscita che deve produrre il programma"
        marcatore_fine = "Note per la consegna"
        
        if marcatore_inizio in testo_pag:
            # Prende tutto ciò che c'è DOPO il marcatore iniziale
            parte_utile = testo_pag.split(marcatore_inizio)[1]
            
            # Se c'è il marcatore di fine, taglia tutto quello che c'è dopo
            if marcatore_fine in parte_utile:
                parte_utile = parte_utile.split(marcatore_fine)[0]
            
            return parte_utile.strip()
        else:
            print("⚠️  Non ho trovato la frase 'Uscita che deve produrre il programma'.")
            print("Provo a restituire l'intera pagina per sicurezza.")
            return testo_pag

    except Exception as e:
        print(f"❌ Errore lettura PDF: {e}")
        return None

def scarica_main_prof(anno, appello, destinazione):
    """Scarica il main.cpp del test"""
    url = f"{BASE_URL}/{anno}_{appello}/main.cpp"
    print(f"⬇️  Scarico Main Test da: {url} ...")
    try:
        response = requests.get(url)
        if response.status_code != 200:
            return False
        with open(destinazione, 'wb') as f:
            f.write(response.content)
        return True
    except:
        return False

def compila_progetto(cartella):
    """Compila ignorando il main dello studente e usando quello del prof"""
    files_cpp = [f for f in os.listdir(cartella) if f.endswith('.cpp')]
    
    sorgenti = []
    for f in files_cpp:
        if f == NOME_MAIN_PROF:
            sorgenti.append(f)
        elif f.lower() == "main.cpp":
            print(f"ℹ️  Ignoro il tuo '{f}' per usare il test del prof.")
        else:
            sorgenti.append(f)
            
    files_path = [os.path.join(cartella, f) for f in sorgenti]
    output_path = os.path.join(cartella, NOME_ESEGUIBILE)
    
    cmd = ["g++", "-o", output_path] + files_path
    res = subprocess.run(cmd, capture_output=True, text=True)
    
    if res.returncode == 0:
        return output_path
    else:
        print("❌ Errore Compilazione:\n" + res.stderr)
        return None

def pulisci_stringa(testo):
    """Rimuove spazi extra e righe vuote per il confronto"""
    if not testo: return ""
    lines = [line.strip() for line in testo.splitlines() if line.strip()]
    return "\n".join(lines)

def main():
    print("--- AUTOMAZIONE ESAME COCOCCIONI (Windows) ---\n")
    
    anno = input("Anno (2016-): ").strip()
    appello = input("Appello (1-7): ").strip()
    
    cartella = seleziona_cartella()
    if not cartella: return

    # 1. SCARICA E MOSTRA OUTPUT ATTESO
    output_atteso = scarica_e_estrai_output(anno, appello)
    if output_atteso:
        print("\n" + "="*10 + " OUTPUT ATTESO (DAL PDF) " + "="*10)
        print(output_atteso)
        print("="*40 + "\n")
    else:
        print("⚠️ Impossibile recuperare l'output atteso dal PDF.")

    # 2. SCARICA MAIN DEL PROF
    path_main_prof = os.path.join(cartella, NOME_MAIN_PROF)
    if not scarica_main_prof(anno, appello, path_main_prof):
        print("❌ Impossibile scaricare il main.cpp del test. Interrompo.")
        return

    # 3. COMPILA ED ESEGUI
    exe = compila_progetto(cartella)
    if exe:
        print("🚀 Eseguo il test...")
        try:
            res = subprocess.run([exe], capture_output=True, text=True, timeout=10)
            output_tuo = res.stdout
            
            print("\n" + "="*10 + " IL TUO OUTPUT " + "="*10)
            print(output_tuo)
            print("="*40)
            
            # 4. VERIFICA AUTOMATICA
            if output_atteso:
                clean_atteso = pulisci_stringa(output_atteso)
                clean_tuo = pulisci_stringa(output_tuo)
                
                # Controllo se l'atteso è contenuto nel tuo (più sicuro dell'uguaglianza perfetta)
                if clean_atteso in clean_tuo:
                    print("\n✅ RISULTATO: SUPERATO! L'output corrisponde al PDF.")
                else:
                    print("\n❌ RISULTATO: FALLITO. C'è qualche differenza.")
                    # Qui potremmo aggiungere una libreria per mostrare le differenze riga per riga
        except subprocess.TimeoutExpired:
            print("❌ Loop Infinito (Timeout)")
        except Exception as e:
            print(f"❌ Errore runtime: {e}")

    # Pulizia file temporanei
    if os.path.exists(path_main_prof):
        os.remove(path_main_prof)
        os.remove(f"{cartella}/{NOME_ESEGUIBILE}")
        print("\n🧹 File di test temporanei rimossi.")

if __name__ == "__main__":
    main()