Function Get-DNSClientCache($pwlang){  
    $hash = @{}  

    Invoke-Expression "ipconfig /displaydns" |  
    Select-String -Pattern $pwlang -Context 0,5 |  
        %{  
			$xml += "<SAAS>`n"
			$xml += "<ENTRY>" + ($_.Line -Split ":")[1].Trim().ToLower()  + "</ENTRY>`n"
			$xml += "<DATA>" + ($_.Context.PostContext[4] -Split ":")[1].Trim().ToLower() + "</DATA>`n"
			$xml += "<TTL>" + ($_.Context.PostContext[1] -Split ":")[1].Trim() + "</TTL>`n"
			$xml += "</SAAS>`n"           
        }  
	
	return $xml
	
} 

$lang = Get-Culture

if($lang.Name -eq "fr-FR"){
	$pwlang = "Nom d'enregistrement"
}else{
	$pwlang = "Record Name"
}

 
$dns = Get-DnsClientCache($pwlang) 
echo $dns