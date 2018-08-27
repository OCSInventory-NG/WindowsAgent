$store = Get-DnsClientCache

foreach ($store in $store) {

	$xml += "<SAAS>`n"
	$xml += "<ENTRY>" + $store.Entry + "</ENTRY>`n"
	$xml += "<DATA>" + $store.Data + "</DATA>`n"
	$xml += "<TTL>" + $store.TimeToLive + "</TTL>`n"
	$xml += "</SAAS>`n"
	
}
	
echo $xml