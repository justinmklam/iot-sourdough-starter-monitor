$('#inputDateRange').daterangepicker({
    timePicker: true,
    timePicker24Hour: true,
    startDate: moment().startOf('hour').subtract(48, 'hour'),
    endDate: moment(),
    maxDate: moment(),
    locale: {
        format: 'MMM D, YYYY h:mm A'
    }
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

            charts.distance.data = MG.convert.date(JSON.parse(data.distance), 'date');
            charts.temperature.data = MG.convert.date(JSON.parse(data.temperature), 'date');
            charts.humidity.data = MG.convert.date(JSON.parse(data.humidity), 'date');

            MG.data_graphic(charts.distance)
            MG.data_graphic(charts.temperature)
            MG.data_graphic(charts.humidity)
        })
  });
