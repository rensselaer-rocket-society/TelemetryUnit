function initiateSaveToDisk(){
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "/persist", true);
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4) {
            if(this.status == 204){
                alert("Saved Successfully");
            }
            else if(this.status == 403) {
                alert("Saving not available in view mode")
            }
        }
    };
    xhttp.send();
}



var persistIndicator = document.createElement('div')
persistIndicator.setAttribute("class", "ls-indicator")
icon = document.createElement('a')
icon.setAttribute("class", "icon-save")
icon.setAttribute("onclick", "initiateSaveToDisk()")
persistIndicator.appendChild(icon)

function HistoricalTelemetryPlugin() {
    return function (openmct) {
        var provider = {
            supportsRequest: function (domainObject) {
                return domainObject.type === 'rrs.mmd';
            },
            request: function (domainObject, options) {
                var url = '/history/' +
                    domainObject.identifier.key +
                    '?start=' + options.start +
                    '&end=' + options.end;

                return http.get(url)
                    .then(function (resp) {
                        return resp.data;
                    });
            }
        };
        openmct.telemetry.addProvider(provider);
    }
}