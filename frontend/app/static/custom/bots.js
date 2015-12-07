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
            var check = $('input:checkbox[name=botcb]:checked').get();
            $("#tb").html("");
            $.each(data, function(i, bot) {
                $("#tb").append('<tr id="row' + i + '"><td><input type="checkbox" name="botcb" value="' + i + '"> ' + i + "</td><td>" + bot.IP + "</td><td>" + bot.OS + "</td><td id=\"intervaltd\">" + bot.set_interval + "</td><td>" + Math.round(bot.last_seen) + "</td></tr>");
                if ($.inArray('killkillkill', bot.cmd_queue) == -1 && bot.last_command != "killkillkill" && bot.current_command != "killkillkill"){
                    $("#row"+i).dblclick(function(e){
                        e.preventDefault();
                        $('.modal-title').html(bot.IP);
                        set_modal_data(i, bot);
                        $('#modal-pop').modal('toggle');
                    });
                } else {
                    $('#row'+i+">td").wrapInner('<s></s>');
                }
                if (($("#modal-pop").data('bs.modal') || {}).isShown && $('#botid').val() == i){
                    set_modal_data(i, bot);
                }
            });
            $.each(check, function(){
                $('input:checkbox[name=botcb][value=' + this.value + ']').prop("checked", true);
            });
        }
    });
}

function get_symbol(bot){
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
    return symbol;
}

function set_modal_data(i, bot){
    var q = "";
    $.each(bot.cmd_queue, function(j, cmd){
        q += j+1 + ". " + cmd + "\n";
    });
    $('#cmdq').html(q);
    $('#botid').val(i);
    $('#lastResponse').html("Current Command: " + bot.current_command + "\nLast Command: " + bot.last_command + "\nResponse:\n" + bot.last_response);
    $('#cliOS').html("<i class=\"fa " + get_symbol(bot) + "\"></i> " + bot.OS);
}

function modal_tabs(e) {
    var target = $(e.target).attr("href")
    if (target == "#last-response" || target == "#command-queue"){
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
        target = "/api/clients/cmd/" + $('#botid').val();
        jdata = JSON.stringify({'cmd': $('#command').val()});
    }
    if (target == "sleep-interval"){
        target = "/api/clients/sleep/" + $('#botid').val();
        jdata = JSON.stringify({'interval': $('#interval').val()});
    }
    $.ajax({
        url: target,
        type: 'POST',
        dataType: 'text',
        contentType: "application/json",
        data: jdata,
        success: function (data, textStatus, xhr) {
            $('.nav-tabs a[href="#last-response"]').tab('show')
            $('#interval').val("");
            $('#command').val("");
            if (target == "sleep-interval"){
                $("td.intervaltd").val($('#interval').val());
            }
        }
    });
}

function delete_bot(e){
    $.ajax({
        url: '/api/clients/delete/' + $('#botid').val(),
        type: 'GET',
        success: function (data, textStatus, xhr) {
            $('#modal-pop').modal('toggle');
        }
    });
}

function kill_bot(e){
    $.ajax({
        url: '/api/clients/kill/' + $('#botid').val(),
        type: 'GET',
        success: function (data, textStatus, xhr) {
            $('#modal-pop').modal('toggle');
        }
    });
}

function clearq_bot(e){
    $.ajax({
        url: '/api/clients/clearq/' + $('#botid').val(),
        type: 'GET',
        success: function (data, textStatus, xhr) {
            $('#cmdq').html("");
            $('#clearqBot').toggleClass("btn-info").toggleClass("btn-success").html("Done!").prop("disabled", true);
            setTimeout(function (){
                $('#clearqBot').toggleClass("btn-info").toggleClass("btn-success").html("Clear Queue").prop("disabled", false);
            }, 3000);
        }
    });
}

function select_all_checkboxes(e){
    $('input:checkbox[name="botcb"]').each(function(){
        $(this).prop("checked", $('input:checkbox[name="allbot"]').prop("checked"));
    });
}

function delete_all_checked(e){
    if (!confirm("Are you sure you want to delete all checked bots?")){
        return;
    }
    $('input:checkbox[name="botcb"]:checked').each(function(){
        $.ajax({
            url: '/api/clients/delete/' + $(this).prop("value"),
            type: 'GET',
        });
    });
}

function kill_all_checked(e){
    if (!confirm("Are you sure you want to kill all checked bots?")){
        return;
    }
    $('input:checkbox[name="botcb"]:checked').each(function(){
        $.ajax({
            url: '/api/clients/kill/' + $(this).prop("value"),
            type: 'GET',
        });
    });
}

function clearq_all_checked(e){
    if (!confirm("Are you sure you want to clear the command queue of all checked bots?")){
        return;
    }
    $('input:checkbox[name="botcb"]:checked').each(function(){
        $.ajax({
            url: '/api/clients/clearq/' + $(this).prop("value"),
            type: 'GET',
        });
    });
}

function command_all_checked(e){
    var cmd = prompt("What command would you like to run?");
    $('input:checkbox[name="botcb"]:checked').each(function(){
        $.ajax({
            url: '/api/clients/cmd/' + $(this).prop("value"),
            type: 'POST',
            dataType: 'text',
            contentType: "application/json",
            data: JSON.stringify({'cmd': cmd}),
        });
    });
}

function sleep_all_checked(e){
    var interval = prompt("What do you want to set the sleep interval to?");
    $('input:checkbox[name="botcb"]:checked').each(function(){
        $.ajax({
            url: '/api/clients/sleep/' + $(this).prop("value"),
            type: 'POST',
            dataType: 'text',
            contentType: "application/json",
            data: JSON.stringify({'interval': interval}),
            success: function (data, textStatus, xhr) {
                $("td.intervaltd").val(interval);
            }
        });
    });
}

function submit_on_enter(e){
    if(e.keyCode == 13){
        $("#submit-btn").click();
    }
}
