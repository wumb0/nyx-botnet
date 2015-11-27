function clients_list(){
    $.ajax({
        url: '/api/clients/list',
        type: 'GET',
        dataType: 'json',
        contentType: "application/json",
        success: function (data, textStatus, xhr) {
            $("#tb").html("");
            $.each(data, function(i, bot) {
                $("#tb").append('<tr id="row' + i + '"><td>' + i + "</td><td>" + bot.IP + "</td><td>" + bot.OS + "</td><td>" + Math.round(bot.last_seen) + "</td>");
                $("#row"+i).dblclick(function(e){
                    e.preventDefault();
                    $('.modal-title').html(bot.IP);
                    var symbol = "fa-question";
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
                    $('#cliOS').html("<i class=\"fa " + symbol + "\"></i> " + bot.OS);
                    $('#modal-pop').modal('toggle');
                });
            });
        }
    });
}

