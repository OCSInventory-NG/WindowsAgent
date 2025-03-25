Function Get-DNSClientCache($pwlang){  
    $hash = @{}  

    Invoke-Expression "ipconfig /displaydns" |  
    Select-String -Pattern $pwlang -Context 0,5 |  
        %{
			$xml += "<SAAS>`n"
			if ($_.Line.Contains(":")) {
				$xml += "<ENTRY>" + ($_.Line -Split ":")[1].Trim().ToLower()  + "</ENTRY>`n"
			} else {
				$xml += "<ENTRY />`n"
			}

			if ($_.Context.PostContext[4].Contains(":")) {
				$xml += "<DATA>" + ($_.Context.PostContext[4] -Split ":")[1].Trim().ToLower() + "</DATA>`n"
			} else {
				$xml += "<DATA />`n"
			}

			if ($_.Context.PostContext[1].Contains(":")) {
				$xml += "<TTL>" + ($_.Context.PostContext[1] -Split ":")[1].Trim() + "</TTL>`n"
			} else {
				$xml += "<TTL />`n"
			}

			$xml += "</SAAS>`n"           
        }  
	
	return $xml
	
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
