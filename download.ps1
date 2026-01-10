$year = Read-Host "Di che anno vuoi scaricare la prova?"

while ($year -lt 2016 -or $year -gt (Get-Date).Year) {
    $year = Read-Host "L'anno deve essere tra 2016 e $((Get-Date).Year). Riprova."
}

$validUrl = $false
while (-not $validUrl) {
    $appello = ""
    while ($appello -lt 1 -or $appello -gt 7) {
        $appello = Read-Host "Quale appello? (1-7)"
        if ($appello -lt 1 -or $appello -gt 7) {
            Write-Host "L'appello deve essere un numero tra 1 e 7. Riprova."
        }
    }
    try {
        $testUrl = "https://docenti.ing.unipi.it/m.cococcioni/FdP/fondprog_tests/${year}_${appello}/testo.pdf"
        $null = Invoke-WebRequest -Uri $testUrl -Method Head -UseBasicParsing -ErrorAction Stop
        $validUrl = $true
    } catch {
        Write-Host "L'appello selezionato non e' valido o non esiste per l'anno specificato. Riprova."
    }
}

$solution = Read-Host "Vuoi scaricare anche la soluzione? (s/N)"
if ($solution -eq "s" -or $solution -eq "S") {
    $urls = @(
        "https://docenti.ing.unipi.it/m.cococcioni/FdP/fondprog_tests/${year}_${appello}/testo.pdf",
        "https://docenti.ing.unipi.it/m.cococcioni/FdP/fondprog_tests/${year}_${appello}/compito.h",
        "https://docenti.ing.unipi.it/m.cococcioni/FdP/fondprog_tests/${year}_${appello}/compito.cpp",
        "https://docenti.ing.unipi.it/m.cococcioni/FdP/fondprog_tests/${year}_${appello}/main.cpp"
    )
} else {
    $urls = @(
        "https://docenti.ing.unipi.it/m.cococcioni/FdP/fondprog_tests/${year}_${appello}/main.cpp",
        "https://docenti.ing.unipi.it/m.cococcioni/FdP/fondprog_tests/${year}_${appello}/testo.pdf"
    )
}

Add-Type -AssemblyName System.Windows.Forms
$dialog = New-Object System.Windows.Forms.FolderBrowserDialog
$dialog.Description = "Seleziona la cartella di destinazione"

$topMostForm = New-Object System.Windows.Forms.Form
$topMostForm.TopMost = $true

$result = $dialog.ShowDialog($topMostForm)

$topMostForm.Dispose()

if ($result -eq [System.Windows.Forms.DialogResult]::OK) {
    $destPath = $dialog.SelectedPath
    $destPath = Join-Path -Path $destPath -ChildPath "Appello ${appello} ${year}"
    if (-not (Test-Path -Path $destPath)) {
        New-Item -ItemType Directory -Path $destPath | Out-Null
    }
} else {
    Write-Host "Operazione annullata."
    exit
}

foreach ($url in $urls) {
    try {
        $fileName = Split-Path -Leaf $url
        $outFile = Join-Path -Path $destPath -ChildPath $fileName
        Write-Host "Attempting to download from: $url"
        Invoke-WebRequest -Uri $url -OutFile $outFile -ErrorAction Stop
        Write-Host "Successfully downloaded: $fileName"

        if ($fileName -eq "main.cpp" -and $solution -ne "s" -and $solution -ne "S") {
            $content = Get-Content -Path $outFile -Raw
            $idx = $content.IndexOf("TERZA PARTE", [System.StringComparison]::OrdinalIgnoreCase)
            if ($idx -ge 0) {
                $lastBrace = $content.LastIndexOf("}")
                if ($lastBrace -gt $idx) {
                    $newContent = $content.Substring(0, $idx) + "`n    // TERZA PARTE RIMOSSA AUTOMATICAMENTE`n" + $content.Substring($lastBrace)
                    Set-Content -Path $outFile -Value $newContent -NoNewline
                    Write-Host "Rimossa la TERZA PARTE da main.cpp"
                }
            }
        }
    }
    catch {
        Write-Host "Error downloading from ${url}: $($_.Exception.Message)"
    }
}

if ($solution -ne "s" -and $solution -ne "S") {
    $hPath = Join-Path -Path $destPath -ChildPath "compito.h"
    if (-not (Test-Path -Path $hPath)) {
        $hContent = "#ifndef COMPITO_H`n#define COMPITO_H`n`n// Inserisci qui il codice`n`n#endif // COMPITO_H"
        Set-Content -Path $hPath -Value $hContent
        Write-Host "Creato compito.h"
    }

    $cppPath = Join-Path -Path $destPath -ChildPath "compito.cpp"
    if (-not (Test-Path -Path $cppPath)) {
        $cppContent = "#include ""compito.h""`n#include <iostream>`n`nusing namespace std;`n`n// Inserisci qui il codice"
        Set-Content -Path $cppPath -Value $cppContent
        Write-Host "Creato compito.cpp"
    }

    $cmakePath = Join-Path -Path $destPath -ChildPath "CMakeLists.txt"
    if (-not (Test-Path -Path $cmakePath)) {
        $projName = "Appello_${appello}_${year}"
        $cmakeContent = "cmake_minimum_required(VERSION 4.0)`nproject($projName)`n`nset(CMAKE_CXX_STANDARD 23)`n`nadd_executable($projName main.cpp`n        compito.cpp`n)"
        Set-Content -Path $cmakePath -Value $cmakeContent
        Write-Host "Creato CMakeLists.txt"
    }
}
