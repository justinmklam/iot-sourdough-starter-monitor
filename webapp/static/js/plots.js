const globals = {
    width: 600,
    height: 200,
    full_width: true,
    right: 40,
    xax_count: 4,
    brush: 'x',
};

// Deepcopy
var distance = JSON.parse(JSON.stringify(globals));
var temperature = JSON.parse(JSON.stringify(globals));
var humidity = JSON.parse(JSON.stringify(globals));
var distance_missing = JSON.parse(JSON.stringify(globals));
var temperature_missing = JSON.parse(JSON.stringify(globals));
var humidity_missing = JSON.parse(JSON.stringify(globals));

distance.data = null;
distance.y_label = "rise percent (%)";
distance.target = '#chart-distance';

temperature.data = null;
temperature.y_label = "temperature (°C)";
temperature.color = 'green';
temperature.target = '#chart-temperature';

humidity.data = null;
humidity.y_label = "humidity (%)";
humidity.color = 'red';
humidity.target = '#chart-humidity';

distance_missing.data = null;
distance_missing.chart_type = "missing-data";
distance_missing.missing_text = "Loading...";
distance_missing.y_label = "rise percent (%)";
distance_missing.target = '#chart-distance';

temperature_missing.data = null;
temperature_missing.chart_type = "missing-data";
temperature_missing.missing_text = "Loading...";
temperature_missing.y_label = "temperature (°C)";
temperature_missing.target = '#chart-temperature';

humidity_missing.data = null;
humidity_missing.chart_type = "missing-data";
humidity_missing.missing_text = "Loading...";
humidity_missing.y_label = "humidity (%)";
humidity_missing.target = '#chart-humidity';

let charts = {
    distance,
    temperature,
    humidity
}

document.addEventListener('DOMContentLoaded', function() {
    MG.data_graphic(distance_missing);
    MG.data_graphic(temperature_missing);
    MG.data_graphic(humidity_missing);
}, false);

var inputSession = document.getElementById("inputSessionId");
inputSession.addEventListener("change", getData);

function getData() {
    console.log(inputSession.value)

    let data = {
        sessionId: inputSession.value
    }

    inputSession.disabled = true;

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
    .fail(function() {
        distance_missing.missing_text = "Couldn't Load Data";
        temperature_missing.missing_text = "Couldn't Load Data";
        humidity_missing.missing_text = "Couldn't Load Data";

        MG.data_graphic(distance_missing);
        MG.data_graphic(temperature_missing);
        MG.data_graphic(humidity_missing);
    })
    .always(function() {
        inputSession.disabled = false;
    })
}

// Populate data with whatever's selected
getData();
