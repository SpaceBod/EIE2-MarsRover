$(document).keydown(function(e) {
  if (e.which==65 || e.which==37) {
    $('.left').addClass('pressed'); 
    $('.lefttext').text('LEFT');
    $('.left').css('transform', 'translate(0, 2px)');
  } else if (e.which==38 || e.which==87) {
    $('.up').addClass('pressed');
    $('.uptext').text('FORWARDS');
    $('.left').css('transform', 'translate(0, 2px)');
    $('.down').css('transform', 'translate(0, 2px)');
    $('.right').css('transform', 'translate(0, 2px)');
  } else if (e.which==39 || e.which==68) {
    $('.right').addClass('pressed');
    $('.righttext').text('RIGHT'); 
    $('.right').css('transform', 'translate(0, 2px)'); 
  } else if (e.which==40 || e.which==83) {
    $('.down').addClass('pressed');
    $('.downtext').text('REVERSE');
    $('.down').css('transform', 'translate(0, 2px)');
  }
});

$(document).keyup(function(e) {
  if (e.which==65 || e.which==37) {
    $('.left').removeClass('pressed');
    $('.lefttext').text('');   
    $('.left').css('transform', 'translate(0, 0)');
  } else if (e.which==38 || e.which==87) {
    $('.up').removeClass('pressed');
    $('.uptext').text('');
    $('.left').css('transform', 'translate(0, 0)');
    $('.down').css('transform', 'translate(0, 0)');
    $('.right').css('transform', 'translate(0, 0)');
  } else if (e.which==39 || e.which==68) {
    $('.right').removeClass('pressed'); 
    $('.righttext').text(''); 
    $('.right').css('transform', 'translate(0, 0)');
  } else if (e.which==40 || e.which==83) {
    $('.down').removeClass('pressed');
    $('.downtext').text('');
    $('.down').css('transform', 'translate(0, 0)');
  }
});