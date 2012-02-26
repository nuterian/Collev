define([
  'underscore',
  'backbone'
  ], function(_,Backbone){

  	Solution = Backbone.Model.extend({
  		
  	});

  	Solutions = Backbone.Collection.extend({
  		model: Solution,
  		url: '/api/bench',

      initialize:function(){
      },

      getBySlug: function(slug){
        var ret = false;
        _(this.models).each(function(model){
          console.log(slug+' : '+model.get('slug'));
          if(model.get('slug') == slug){
            ret = model;
          }
        });
        return ret;
      }
  	});

  	return Solutions;
});
  