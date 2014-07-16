var offgrid = require('./build/Release/offgrid');

offgrid.blank()
    .blue(78)
    .blue(77)
    .blue(66)
    .send();

setTimeout(function() {
    offgrid.blank().send();
}, 2000);

