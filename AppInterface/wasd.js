var input = [119, 97, 115, 100];
var br = document.createElement("br");

button = document.createElement("button");
button.id = button.textContent = String.fromCharCode(input[0]);
document.body.appendChild(button);
document.getElementById(button.id).style.marginLeft = "160px";
document.body.appendChild(br);

for(var i = 1; i < 4; i++){
    button = document.createElement("button");
    button.id = button.textContent = String.fromCharCode(input[i]);
    document.body.appendChild(button);
}

var keydown,
    keypress = [];

document.addEventListener("keydown", function onKeydown(e1) {
    keydown = e1;
}, false);

document.addEventListener("keypress", function onKeypress(e2) {
    var record = {
        "char": e2.char || e2.charCode,
        "key": keydown.key || keydown.keyCode || keyDown.which,
        "shiftKey": keydown.shiftKey,
        "metaKey": keydown.metaKey,
        "altKey": keydown.altKey,
        "ctrlKey": keydown.ctrlKey
    },
    element = document.getElementById(String.fromCharCode(e2.charCode || e2.char));

    if (element) {
        element.style.backgroundColor = "#4d88ff";
        keypress.push(record);
    }
    if((e2.charCode == 119) || (e2.charCode == 97) || (e2.charCode == 115) || (e2.charCode == 100)){
    	console.log(String.fromCharCode(e2.charCode));
    	IP = "http://192.168.1.16:80";
		$.get( IP, { "dir": String.fromCharCode(e2.charCode) });
		{Connection: close};
    }
}, false);

document.addEventListener("keyup", function onKeyup(e3) {
    var key = e3.key || e3.keyCode || e3.which;

    keypress.forEach(function (record) {
        if (record.key === key && record.shiftKey === e3.shiftKey && record.metaKey === e3.metaKey && record.altKey === e3.altKey && record.ctrlKey === e3.ctrlKey) {
            document.getElementById(String.fromCharCode(record.char)).style.backgroundColor = "#e7e7e7";
        }
    });
}, false);