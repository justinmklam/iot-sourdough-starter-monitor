$(function () {
    $('input[name="datetimes"]').daterangepicker({
        timePicker: true,
        timePicker24Hour: true,
        startDate: moment().startOf('hour').subtract(48, 'hour'),
        endDate: moment(),
        maxDate: moment(),
        locale: {
            format: 'M/DD HH:mm'
        }
    });
});
