//var ffmpeg = require('fluent-ffmpeg');

//ffmpeg('/Users/jiaoew/Documents/task/frame_seeker/resources/source1.mp4').on('start', function(commandLine) {
    //console.log('Spawned Ffmpeg with command: ' + commandLine);
  //}).on('progress', function(progress) {
    //console.log('Processing: ' + progress.percent + '% done');
  //}).on('codecData', function(data) {
    //console.log('Input is ' + data.audio + ' audio ' +
      //'with ' + data.video + ' video');
  //}).on('error', function(err, stdout, stderr) {
    //console.log('Cannot process video: ' + err.message);
  //}).on('end', function() {
    //console.log('Transcoding succeeded !');
  //});
var Ffmpeg = require('fluent-ffmpeg');

Ffmpeg.getAvailableFormats(function(err, formats) {
  console.log('Available formats:');
  console.dir(formats);
});
