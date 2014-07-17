var offgrid = require('./build/Release/offgrid');

// Selectively re-export and/or wrap offgrid methods.
exports.blank = offgrid.blank;
exports.blue = offgrid.blue;
exports.send = offgrid.send;
