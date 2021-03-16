 var slider = document.getElementById("heightRange");
 var output = document.getElementById("height");
 output.innerHTML = slider.value;

 slider.oninput = function() {
    output.innerHTML = this.value;
    console.log(this.value);
   	ArduinoVar = "http://192.168.1.16:80";
	$.get( ArduinoVar, { "hei": this.value })	;
	{Connection: close};
 }