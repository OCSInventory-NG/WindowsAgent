Function Get-DNSClientCache($pwlang){  
    $hash = @{}  
    $entries = [System.Collections.Generic.List[string]]::new()

    Invoke-Expression "ipconfig /displaydns" |  
    Select-String -Pattern $pwlang -Context 0,5 |  
        %{
			$entries.Add("<SAAS>`n")
			if ($_.Line.Contains(":")) {
				$entries.Add("<ENTRY>" + ($_.Line -Split ":")[1].Trim().ToLower()  + "</ENTRY>`n")
			} else {
				$entries.Add("<ENTRY />`n")
			}

			if ($_.Context.PostContext[4].Contains(":")) {
				$entries.Add("<DATA>" + ($_.Context.PostContext[4] -Split ":")[1].Trim().ToLower() + "</DATA>`n")
			} else {
				$entries.Add("<DATA />`n")
			}

			if ($_.Context.PostContext[1].Contains(":")) {
				$entries.Add("<TTL>" + ($_.Context.PostContext[1] -Split ":")[1].Trim() + "</TTL>`n")
			} else {
				$entries.Add("<TTL />`n")
			}

			$entries.Add("</SAAS>`n")           
        }  
	
	if ($entries.Count -eq 0) {
		return $null
	}
	return -join $entries
	
} 

$lang = Get-Culture

if($lang.Name -eq "fr-FR"){
	$pwlang = "Nom d'enregistrement"
}elseif($lang.Name -eq "it-IT"){
	$pwlang = "Nome record"
}elseif($lang.Name -eq "es-ES"){
	$pwlang = "Nombre de registro"
}else{
	$pwlang = "Record Name"
}

$dns = Get-DnsClientCache($pwlang)
echo $dns
