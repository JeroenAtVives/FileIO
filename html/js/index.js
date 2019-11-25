document.getElementById("getJSONButton").addEventListener("click",myfunction);

// myfunction zal de data van een json bestand proberen op te vragen.
async function myfunction(){

    // De tabel leegmaken...
    document.getElementById("table").innerHTML = "";

    try {
        // Wachten tot de JSON opgehaald is...
        const response = await GetData("http://pijeroen/myJSON.json");
        console.log(JSON.stringify(response));
        console.log(response);
        if(response.length != 0)
        {
            ShowJSONToUser(response);
        }else{
            alert("Er is blijkbaar geen data aanwezig in myJSON.json");
        }
      } catch (error) {
        alert("Er is een probleem opgetreden. Zie console voor meer info.")
        console.log('There has been a problem with your fetch operation: ', error.message);
    }    
}

// Een fetch doen naar een url en de json opvragen.
async function GetData(url) {
    var response = await fetch(url);
    return await response.json(); // Parse JSON naar objects
}

// Functie om de JSON te visualiseren.
function ShowJSONToUser(response){

    // Initialisatie tabel...
    var tabel = document.getElementById("table");
    tabel.setAttribute("class","table-sm table-striped table-hover")
    
    // Initialisatie headerRij
    var headerRij = document.createElement("thead");
    var tr = document.createElement("tr");

    // Stel dat er in de toekomst nog kolommen bijkomen is het 
    // misschien beter om die namen niet hardcoded te typen maar echt gebruik te
    // maken van de veldnamen in het jsonbestand. Daarvoor is er een method getOwnPropertyNames
    // die de properties kan opvragen van een bepaald object en een array van propertynamen teruggeeft.
    var columnNames = ["#"];    
    var columnNames2 = Object.getOwnPropertyNames(response[0]);
    for (let index = 0; index < columnNames2.length; index++) {
        const columnName = columnNames2[index];
        columnNames.push(columnName); 
    }

    // Kolomnamen toevoegen aan de headerrij.
	for (var index = 0; index < columnNames.length; index++) {
    	var th = document.createElement("th");
        th.innerHTML = columnNames[index];
        th.scope = "col";
    	tr.appendChild(th);
    }
    headerRij.appendChild(tr);

    // De effectieve data gaan toevoegen aan de tabel, het vullen van tbody...
    var tbody = document.createElement("tbody");
    var rowNummer = 0;

    // Elk object toevoegen aan de tabel...
    response.forEach(element => {
        var tr = document.createElement("tr");

        var th = document.createElement("th");
        th.innerHTML = ++rowNummer;  // Direct +1 doen.
        th.scope = "row"
        tr.appendChild(th);

        // De values opvragen die horen bij de properties en ze in een cel van de tabel zetten...
        for (let index = 0; index < Object.values(element).length; index++) {
            const datacel = Object.values(element)[index];
            var td = document.createElement("td");
            td.innerHTML = datacel;
            tr.appendChild(td);
        }

        tbody.appendChild(tr);

    });

    
	
	tabel.appendChild(headerRij);
	tabel.appendChild(tbody);

	
	document.body.appendChild(tabel);
}