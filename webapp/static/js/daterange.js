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
  });
