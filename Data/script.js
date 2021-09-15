//var times = "";

//let time = '{"Automode":[{ "hora": "08:00", "tamanho": "M", "state": "checked"},{ "hora": "12:00", "tamanho": "M", "state": ""},{ "hora": "16:00", "tamanho": "M", "state": "checked"},{ "hora": "20:00", "tamanho": "M", "state": ""},{ "hora": "22:00", "tamanho": "G", "state": "checked"}]}';
//var times = JSON.parse(time);
receberPacote();

function enviarManual() {
    var getSelectedValue = document.querySelector(
        'input[name="tamanhoPorcao1"]:checked');
    pacoteManual = ("/manual?tamanho=" + getSelectedValue.value);
    enviarPacote(pacoteManual);
    atualizarNivel();
}

function enviarAuto() {
    var getSelectedValue = document.querySelector(
        'input[name="tamanhoPorcao2"]:checked');
    var getText1 = document.getElementById('horaInicio').value;

    pacoteAuto = ("/auto?tamanho=" + getSelectedValue.value + "&inicio=" + getText1);
    enviarPacote(pacoteAuto);
    receberPacote();
}

function enviarPacote(x) {
    if (x != null) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", x, true);
        xhr.send();
    }
}

$(document).ready(function() {
    atualizarNivel();
    //atualizarSaves();
});


function atualizarNivelR() {
    $(".tk").each(function() {
        var amount = $('.lq').attr('data-amount')
        var quantity = amount;
        $(this).find('.lq').animate({ 'height': parseInt(amount) + '%' }, 1000);
        $('.ring').css({ height: 100 - amount + 10 + '%' });
        $('.text').text(quantity + '%');
    });
    $('.text').each(function() {
        var $this = $(this);
        jQuery({ Counter: 0 }).animate({ Counter: $this.text() }, {
            duration: 1000,
            easing: 'swing',
            step: function() {
                $this.text(Math.ceil(this.Counter) + "%");
            }
        });
    });
}


function atualizarNivel() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            //document.getElementById("teste").innerHTML = this.responseText;
            $("#nivel").attr("data-amount", this.responseText);
            atualizarNivelR();
        }
    };
    xhttp.open("GET", "/nivel", true);
    xhttp.send();
}

function atualizarSaves(times) {
    let text = "";
    for (let i = 0; i < Object.keys(times.Automode).length; i++) {
        text += '<div class="w3-container" id=autoconfig' + i + '> <span class="w3-text-white w3-large w3-left">' + times.Automode[i].hora + '</span><label class="switch w3-right"><input id=autoconfigChk' + i + ' type="checkbox" onchange="atualizarLista(' + i + ')"' + times.Automode[i].state + '><span class="slider round"></span></label></div>';
    }

    document.getElementById("autoTimes").innerHTML = text;
    //$("div[target='displayTimes']").replaceWith(text);
}

function receberPacote() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == XMLHttpRequest.DONE) {
            atualizarSaves(JSON.parse(this.responseText));

        }
    };
    xhttp.open("GET", "/pacote", true);
    xhttp.send();
}


function atualizarLista(i) {
    var chkBox = document.getElementById('autoconfigChk' + i + '');
    if (chkBox.checked) {
        chk[i] = "checked";
    } else {
        chk[i] = "";
    }
    montarPacote();
}