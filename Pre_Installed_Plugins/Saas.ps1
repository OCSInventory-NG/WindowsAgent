Function Get-DNSClientCache{  
    $hash = @{}  

    Invoke-Expression "ipconfig /displaydns" |  
    Select-String -Pattern "Nom d'enregistrement" -Context 0,5 |  
        %{  
			$xml += "<SAAS>`n"
			$xml += "<ENTRY>" + ($_.Line -Split ":")[1].Trim().ToLower()  + "</ENTRY>`n"
			$xml += "<DATA>" + ($_.Context.PostContext[4] -Split ":")[1].Trim().ToLower() + "</DATA>`n"
			$xml += "<TTL>" + ($_.Context.PostContext[1] -Split ":")[1].Trim() + "</TTL>`n"
			$xml += "</SAAS>`n"           
        }  
	
	return $xml
	
} 
 
$dns = Get-DnsClientCache  
echo $dns