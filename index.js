var offgrid = require('./build/Debug/offgrid');

offgrid.init(575);

// Selectively re-export and/or wrap offgrid methods.
exports.blank = offgrid.blank;
exports.red = offgrid.red;
exports.green = offgrid.green;
exports.blue = offgrid.blue;
exports.write = offgrid.write;
exports.send = offgrid.send;
