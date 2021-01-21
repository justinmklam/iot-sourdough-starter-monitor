

$("#inputSessionId").on("change", function() {
    console.log("Hello")

});


$('#inputDateRange').on('apply.daterangepicker', function(ev, picker) {
    console.log(picker.startDate.format("YYYY-MM-DD HH:mm"))
    console.log(picker.endDate.format("YYYY-MM-DD HH:mm"))

    let data = {
        start: picker.startDate.format("YYYY-MM-DD HH:mm"),
        end: picker.endDate.format("YYYY-MM-DD HH:mm")
    }

    $.post("/get_data", data)
        .done(function(data){
            console.log(data)

            charts.rise.data = MG.convert.date(JSON.parse(data.risepercent), 'time');
            charts.temperature.data = MG.convert.date(JSON.parse(data.temperature), 'time');
            charts.humidity.data = MG.convert.date(JSON.parse(data.humidity), 'time');

            MG.data_graphic(charts.rise)
            MG.data_graphic(charts.temperature)
            MG.data_graphic(charts.humidity)
        })
  });
