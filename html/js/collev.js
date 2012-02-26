require.config({
  paths: {
    jquery: 'lib/jquery.min',
    underscore: 'lib/underscore-min',
    backbone: 'lib/backbone-optamd3-min',
    text: 'lib/text'
  }

});
	
require(['backbone','views/appView'], function(Backbone, App){
	app = new AppView();
});