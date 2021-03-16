var count = 1;
function changeColor(power) {
    var property = document.getElementById(power);
    if (count == 0) {
        property.style.backgroundColor = "#4679BD"
        count = 1;   
        console.log("ON")
        IP = "http://192.168.1.16:80";
        $.get( IP, { "pow": "ON" })  ;
        {Connection: close};     
    }
    else {
        property.style.backgroundColor = "#9cadab"
        count = 0;
        console.log("OFF")
        IP = "http://192.168.1.16:80";
        $.get( IP, { "pow": "OFF" })  ;
        {Connection: close};
    }
}