
host = "mqtt.eclipseprojects.io";
topic = "remote_switch_topic";
msg = "ToggleLED";
port = 80;

function startConnect(){
   clientID = "clientID - "+parseInt(Math.random() * 100);

   client = new Paho.MQTT.Client(host,Number(port),clientID);
   
   client.onConnectionLost = onConnectionLost;

   client.connect({
      onSuccess: onConnect
   });
}

function onConnect(){
   document.getElementById("messages").innerHTML += "<span> Connected.</span><br>";
}

function startDisconnect(){
   client.disconnect();
}

function onConnectionLost(responseObject){
   if (responseObject != 0){
       document.getElementById("messages").innerHTML += "<span> Disconnected: "+ responseObject.errorMessage +"</span><br>";
   }
   else {
      document.getElementById("messages").innerHTML += "<span> Disconnected.</span><br>";
   }
}

function publishMessage(){
   Message = new Paho.MQTT.Message(msg);
   Message.destinationName = topic;

   client.send(Message);
   document.getElementById("messages").innerHTML += "<span>LED toggled.</span><br>";
}
