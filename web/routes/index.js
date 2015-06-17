var express = require('express');
var multer = require('multer');
var path = require('path');
var similar = require('../../src/similar');

var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
    res.render('index', { title: 'hello' });
});

router.get('/query', function(req, res, next) {

    var videoFilePath = path.join(__dirname, '..', 'public/uploads', req.query.video);
    var imageFilePath = path.join(__dirname, '..', 'public/uploads', req.query.image);
    var outputPath = path.join(__dirname, '..', 'public/img') + '/';
    console.log('video file', videoFilePath);
    similar.findSimilar(videoFilePath, imageFilePath, outputPath, 5, function(results) {
        results = results.map(function(elem) {
            elem.filename = '/img/' + elem.filename + '.jpg'
            return elem;
        });
        res.json({
            data: results
        });
    });
    //res.json({
        //data: [
            //{
                //filename: 'img/tmp_41.jpg',
                //distance: 0
            //},
            //{
                //filename: 'img/tmp_42.jpg',
                //distance: 4
            //},
            //{
                //filename: 'img/tmp_43.jpg',
                //distance: 4
            //},
            //{
                //filename: 'img/tmp_44.jpg',
                //distance: 8
            //},
            //{
                //filename: 'img/tmp_46.jpg',
                //distance: 6
            //}
        //]
    //})
});

router.post('/upload', [multer({ dest: './public/uploads'}), function(req, res, next) {
    console.log(req.files)
    res.json({
        path: req.files['file'].name
    });
}])

module.exports = router;
