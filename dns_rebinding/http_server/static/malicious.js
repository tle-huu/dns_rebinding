
console.log("Malicious script starting")

function malicious()
{



fetch("http://proute.caca:4242", {credentials: 'omit'}).then( (res) => res.text()).then( (html) => {alert(html); setTimeout(malicious, 3000);})
        .catch((err) => {
                console.log("Could not fetch: ", err)
                setTimeout(malicious, 3000)
            }
    );

}

malicious()
console.log("Malicious script ending")
