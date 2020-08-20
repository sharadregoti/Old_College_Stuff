// fetch('http://bridge-monitoring.herokuapp.com/client')
//     .then(function (response) {
//         if (response == null) {
//             console.log("Didn't Recieved HTML");
//             return
//         }
//     })

var socket = new WebSocket("ws://localhost:8000/ws");
socket.onmessage = function (event) {
    console.log(event.data);
    var match = event.data.split(',')
    console.log(match[0].charAt(1))
    console.log(match[1])
    var parts = match[1].lastIndexOf('\\');
    var result = match[1].substring(parts + 1)
    // var parts = match[1].split("\\");
    // var result = parts[0].fontsize(15);
    switch (match[0].charAt(1)) {
        case "1":
            document.getElementById("1").innerHTML = result;
            break;
        case "2":
            document.getElementById("2").innerHTML = result;
            break;
        case "3":
            document.getElementById("3").innerHTML = result;
            break;
        case "4":
            document.getElementById("4").innerHTML = result;
            break;
        case "5":
            document.getElementById("5").innerHTML = result;
            break;
        case "6":
            document.getElementById("6").innerHTML = result;
            break;
        default:
            console.log("invalid staring point");
    }
}