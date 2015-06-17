
var similar = require('./similar');

console.log('start at ', Date.now())
similar.findSimilar('../resources/source5.mp4', '../resources/3.jpg', './build/fuck/', 5, function(rtn) {
    console.log('result is ', rtn);
    console.log('end at ', Date.now())
})
