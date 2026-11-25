# just_ascii.ps1

$dirs = @('include', 'src', 'singleheader', 'tests', 'examples')
$nonAscii = Get-ChildItem -Path $dirs -Recurse -File | Where-Object {
    $_.Extension -match '\.(hpp|cpp)$'
} | Where-Object {
    try {
        $bytes = [System.IO.File]::ReadAllBytes($_.FullName)
        $skip = if ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF) { 3 } else { 0 }
        for ($i = $skip; $i -lt $bytes.Length; $i++) {
            if ($bytes[$i] -gt 127) { return $true }
        }
        return $false
    }
    catch { return $false }
}

if ($nonAscii.Count -gt 0) {
    Write-Host 'The following files contain non-ASCII characters:'
    $nonAscii | ForEach-Object { Write-Host $_.FullName }
    exit 1
}
exit 0
