const globals = {
    width: 600,
    height: 200
};

let charts = {
    distance: {
        data: null,
        y_label: "rise percent (%)",
        width: globals.width,
        height: globals.height,
        full_width: true,
        right: 40,
        xax_count: 4,
        brush: 'x',
        target: '#chart-distance',
    },
    temperature: {
        data: null,
        width: globals.width,
        height: globals.height,
        y_label: "temperature (°C)",
        full_width: true,
        color: 'green',
        right: 40,
        xax_count: 4,
        brush: 'x',
        target: '#chart-temperature',
    },
    humidity: {
        data: null,
        width: globals.width,
        height: globals.height,
        y_label: "humidity (%)",
        full_width: true,
        color: 'red',
        right: 40,
        xax_count: 4,
        brush: 'x',
        target: '#chart-humidity',
    }
}

var inputSession = document.getElementById("inputSessionId");
inputSession.addEventListener("change", getData);

function getData() {
    console.log(inputSession.value)

    let data = {
        sessionId: inputSession.value
    }

    $.post("/get_data", data)
    .done(function(data){
        console.log(data)

        charts.distance.data = MG.convert.date(JSON.parse(data.risepercent), 'date', "%Y-%m-%d %H:%M:%S");
        charts.humidity.data = MG.convert.date(JSON.parse(data.humidity), 'date', "%Y-%m-%d %H:%M:%S");
        charts.temperature.data = MG.convert.date(JSON.parse(data.temperature), 'date', "%Y-%m-%d %H:%M:%S");

        MG.data_graphic(charts.distance)
        MG.data_graphic(charts.temperature)
        MG.data_graphic(charts.humidity)
    })
}

// Populate data with whatever's selected
getData();
