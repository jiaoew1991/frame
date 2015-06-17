var fs = require('fs-extra');
var lwip = require('lwip');
var video = require('./build/Release/video');
var phash = require('./phash');

exports.findSimilar = function(videoFilePath, imageFilePath, outputPath, size, callback) {
    console.log('video path is ', videoFilePath);
    var videoHelper = new video.Video(videoFilePath, outputPath);
    lwip.open(imageFilePath, function(err, image) {
        image.resize(320, function(err, image) {
            var originHash = phash.imageHashSync(imageFilePath);
            var resultList = [];
            videoHelper.onProgress(function(filename) {
                var destHash = phash.imageHashSync(outputPath + "tmp.jpg");
                var haming = phash.hammingDistance(originHash, destHash);
                //console.log('haming distance is ', haming);
                fs.copySync(outputPath + 'tmp.jpg', outputPath + filename + '.jpg');
                resultList.push({
                        filename: filename,
                        distance: haming
                });
                resultList = resultList.sort(function(a, b) {
                    return a.distance - b.distance;
                });
                if (resultList.length > size) {
                    var useless = resultList.pop();
                    console.log('useless one is ', useless);
                    fs.removeSync(outputPath + useless.filename + '.jpg');
                }
            }).start();
            callback(resultList);
        });
    });
};
