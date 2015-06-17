$(function() {
    var videoFile = '';
    var imageFile = '';

    $('.drop a').click(function(){
        // Simulate a click on the file input button
        // to show the file browser dialog
        $(this).parent().find('input').click();
    });

    // Initialize the jQuery File Upload plugin
    $('.upload-form').fileupload({

        // This element will accept file drag/drop uploading
        dropZone: $('.drop'),

        // This function is called when a file is added to the queue;
        // either via the browse button, or via drag/drop:
        add: function (e, data) {

            console.log(data);
            var tpl = $('<li class="working"><input type="text" value="0" data-width="48" data-height="48"'+
                ' data-fgColor="#0788a5" data-readOnly="1" data-bgColor="#3e4043" /><p></p><span></span></li>');

            // Append the file name and file size
            tpl.find('p').text(data.files[0].name)
                         .append('<i>' + formatFileSize(data.files[0].size) + '</i>');

            // Add the HTML to the UL element
            data.context = tpl.appendTo($(this).find('ul'));

            // Initialize the knob plugin
            tpl.find('input').knob();

            // Listen for clicks on the cancel icon
            tpl.find('span').click(function(){

                if(tpl.hasClass('working')){
                    jqXHR.abort();
                }

                tpl.fadeOut(function(){
                    tpl.remove();
                });

            });
            var $this = $(this);

            // Automatically upload the file once it is added to the queue
            var jqXHR = data.submit();
            jqXHR.success(function(res, textStatus, jqXHR) {
                console.log(res);
                if ($this.data('type') == 'video') {
                    videoFile = res.path;
                } else {
                    imageFile = res.path;
                }
            });
        },

        progress: function(e, data){

            // Calculate the completion percentage of the upload
            var progress = parseInt(data.loaded / data.total * 100, 10);

            // Update the hidden input field and trigger a change
            // so that the jQuery knob plugin knows to update the dial
            data.context.find('input').val(progress).change();

            if(progress == 100){
                data.context.removeClass('working');
            }
        },

        fail:function(e, data){
            // Something has gone wrong!
            data.context.addClass('error');
        },

    });

    $('#query-button').on('click', function(e) {
        e.preventDefault();
        $.get('/query', {
            'video': videoFile,
            'image': imageFile
        }, function(res) {
            var index = 1;
            res.data.forEach(function(elem) {
                var tpl = $('<li><a href="#result-' + index + '" rel="modal:open"><img style="max-width:48px;" src="' + elem.filename + '" alt=""/><p></p><span></span></a></li>');

                // Append the file name and file size
                tpl.find('p').text(elem.filename).append('<i>汉明距离: ' + elem.distance  + '</i>');
                tpl.appendTo($('#result-area').find('ul'));
                $('#result-area').append('<form id="result-' + index + '" class="modal">' +
                    '<img src="' + elem.filename + '"/><img src="uploads/' + imageFile + '" />'
                +'</form>')
                index += 1;
            });

            console.log(res);
        });
    });

    // Prevent the default action when a file is dropped on the window
    $(document).on('drop dragover', function (e) {
        e.preventDefault();
    });

    // Helper function that formats the file sizes
    function formatFileSize(bytes) {
        if (typeof bytes !== 'number') {
            return '';
        }

        if (bytes >= 1000000000) {
            return (bytes / 1000000000).toFixed(2) + ' GB';
        }

        if (bytes >= 1000000) {
            return (bytes / 1000000).toFixed(2) + ' MB';
        }

        return (bytes / 1000).toFixed(2) + ' KB';
    }
});
