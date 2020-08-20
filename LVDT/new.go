package main

import (
	"bufio" // used for taking inputs from user or port
	"fmt"
	"log"
	"net" //libaray use for print function
	"net/http"
	// used for net
	"os"
	"strconv"
	"strings"
	"time"

	"github.com/gorilla/mux"
	"github.com/gorilla/websocket"
	// used to read user inputs
)

var channel = make(chan []string)

func main() {
	go startServer()
	router := mux.NewRouter()
	router.HandleFunc("/ws", simpleHandler)
	router.HandleFunc("/", handler)
	router.PathPrefix("/").Handler(http.FileServer(http.Dir("./")))
	fmt.Println("Starting Server on 8000")
	err := http.ListenAndServe(":8000", router)
	if err != nil {
		panic(err)
	}
}

// on client request sends index.html
func handler(w http.ResponseWriter, r *http.Request) {
	http.ServeFile(w, r, "index.html")
}

func simpleHandler(w http.ResponseWriter, r *http.Request) {
	upgrader := websocket.Upgrader{}
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Fatal("Upgrading To Websocket Error : ", err)
		return
	}
	// read from connection, only to check if connection is alive
	go readClientSocket(conn)

	// write from connection
	for {
		select {
		case data := <-channel:
			{
				err := conn.WriteJSON(data[0] + "," + data[2])
				if err != nil {
					fmt.Println("Error Writing Data to Client (Web browser)")
					return
				}
			}
		}

	}
}

func readClientSocket(conn *websocket.Conn) {
	for {
		_, _, err := conn.ReadMessage()
		if err != nil {
			fmt.Println("Error in reading from Client(Web browser)")
			return
		}
	}
}

// starts a tcp server, accepts all incoming connection
func startServer() {
	fmt.Println("Start")
	l, err := net.Listen("tcp", ":8090") //set a server listening at port 8090
	if err != nil {
		fmt.Println(err)
	}
	for {
		conn, err := l.Accept() // accept incoming client connection
		if err != nil {
			fmt.Println(err)
		}
		go res(conn) //function for reading data from client
	}
}

func res(conn net.Conn) {
	reader := bufio.NewReader(conn) // set a new reader for tcp
	// If the file doesn't exist, create it, or append to the file
	file25, err := os.OpenFile("LVDT25.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatal(err)
	}
	defer file25.Close() // Make sure to close the file when you're done
	file50, err := os.OpenFile("LVDT50.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatal(err)
	}
	defer file50.Close() // Make sure to close the file when you're done

	for {

		text, err := reader.ReadString('\n')
		fmt.Println("Text", text)
		read_line := strings.TrimSuffix(strings.TrimSuffix(text, "\n"), "\r")
		fmt.Println("Length", len(text), len(read_line))
		firstCharacter := strings.Split(text, ",")
		if err != nil {
			fmt.Println("Client DISCONNECTED")
			return
		} else {
			t := time.Now()
			val := t.Format("Mon Jan _2 15:04:05 2006")
			data := read_line + "," + val
			fmt.Println("else", data)

			if firstCharacter[0] > "3" {
				x, err := strconv.ParseFloat(strings.TrimSpace(firstCharacter[2]), 64)
				x = (x * 25) / 1024
				str := strconv.FormatFloat(x, 'f', 6, 64)
				firstCharacter[2] = str
				channel <- firstCharacter
				_, err = file25.WriteString(data + ", Data , " + str + "\n")
				if err != nil {
					log.Fatalf("failed writing to file: %s", err)
				}
			} else {
				fmt.Println(firstCharacter[2])
				x, err := strconv.ParseFloat(strings.TrimSpace(firstCharacter[2]), 64)
				x = (x * 50) / 1024
				str := strconv.FormatFloat(x, 'f', 6, 64)
				firstCharacter[2] = str
				channel <- firstCharacter
				_, err = file50.WriteString(data + ", Data ," + str + "\n")
				if err != nil {
					log.Fatalf("failed writing to file: %s", err)
				}
			}

		}
	}
}
