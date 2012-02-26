define([
  'jquery',
  'underscore', 
  'backbone',
  'models/solutions'
  ], function($, _, Backbone, Solutions){

  	SolutionView = Backbone.View.extend({
  		tagName: "div",
  		className: "solution",

  		initialize: function(){
  			_.bindAll(this, 'render');
  		},

  		render: function(){
  			$(this.el).html('<h3><a href="#/'+this.model.get('slug')+'">'+this.model.get('name')+'</a></h3><div>'+this.model.get('challenge')['name']+'</div>');
  			return this;
  		}
  	});

  	SolutionListView = Backbone.View.extend({
  		id: "solutionList",
      class: "column",

  		initialize: function(){
  			_.bindAll(this, 'render', 'reposition', 'load', 'show', 'hide');
  			$(window).bind('resize', this.reposition);
  			this.hide();
  			this.collection = new Solutions();
  		},

  		render:function(){
  			$(this.el).html('<h2>Recent Challenges</h2>');
  			_(this.collection.models).each(function(solution){
  				console.log(solution.get("name"));
  				var solutionView = new SolutionView({model: solution});
  				$(this.el).append(solutionView.render().el);
  			}, this);
  			this.show();
  		},

  		load: function(){
  			this.collection.fetch({success:this.render});
  			this.reposition();
  			return this;
  		},

  		show: function(){
  			$(this.el).show();
  		},

  		hide: function(){
  			$(this.el).hide();
  		},

  		reposition:function(){
  			var winWidth = $(window).width();
  			var winHeight = $(window).height();

  			// The View must be positioned such that it is in the center of the window.
  			var posLeft = winWidth/2 - $(this.el).width()/2;
  			var posTop = winHeight/2 - $(this.el).height()/2;

  			$(this.el).css("left", posLeft+'px');
  			$(this.el).css("top", posTop+'px');
  		}
  	});

  	return SolutionListView;
 });