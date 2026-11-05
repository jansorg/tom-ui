# Fails unless all given binaries have a valid Authenticode signature.
$failed = $false
foreach ($binary in $args) {
    $signature = Get-AuthenticodeSignature -FilePath $binary
    Write-Host "--> ${binary}: $($signature.Status) $($signature.SignerCertificate.Subject)"
    if ($signature.Status -ne 'Valid') {
        Write-Host "::error::${binary}: Authenticode signature is $($signature.Status): $($signature.StatusMessage)"
        $failed = $true
    }
}
if ($failed) { exit 1 }
