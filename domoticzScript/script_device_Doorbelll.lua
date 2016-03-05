DomDevice = 'Doorbell'

Title = 'Doorbell rings'
Body = 'There is someone at the door'

AccessToken = 'insert Pushbullet API key here'
ChannelTag = 'insert Pushbullet channeltag'
Number = 'insert receiving Whatsapp-number'

commandArray = {}

if devicechanged[DomDevice] == 'On' then
	runcommand = "curl -u " .. (AccessToken) .. ":  https://api.pushbullet.com/v2/pushes -d type=note -d channel_tag=" .. (ChannelTag) .. " -d title='" .. (Title) .. "' -d body='" .. (Body) .. "'";
   	print ("Alert sent to Pushbullet");
	os.execute(runcommand);

	runcommand1 = "yowsup-cli demos -c /home/domoticz/yowsup-master/config.example -s " .. (Number) .. " '" .. (Body) .. "'";
   	print ("Alert sent to Whatsapp");
	test = os.execute(runcommand1);
	print (test);
end

return commandArray