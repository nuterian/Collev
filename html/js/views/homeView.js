define([
  'jquery',
  'underscore', 
  'backbone',
  ], function($, _, Backbone){

    SolutionItemView = Backbone.View.extend({
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
  		id: "solutionListView",

  		initialize: function(){
  			_.bindAll(this, 'render', 'reposition', 'show', 'hide');

        this.options.width = 400;
        this.options.height = 350;

  			$(window).bind('resize', this.reposition);
  			this.hide();
  		},

  		render:function(){
  			$(this.el).html('<h2>Recent Challenges</h2>');
        $(this.el).append('<div id="solutionList"></div>');
  			_(this.collection.models).each(function(solution){
  				console.log(solution.get("name"));
  				var solutionView = new SolutionItemView({model: solution});
          solutionView.bind('selected', function(solution){self.trigger('solutionSelected', solution)});
  				$("#solutionList", this.el).append(solutionView.render().el);
  			}, this);
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
  			var posLeft = winWidth/2 - this.options.width/2;
  			var posTop = winHeight/2 - this.options.height/2;

        posTop = (posTop<50)?50:posTop;

  			$(this.el).css("left", posLeft+'px');
  			$(this.el).css("top", posTop+'px');
  		}
  	});

    HomeView = Backbone.View.extend({
      id: "homeView",

      initialize: function(){
        this.solutionList = new SolutionListView({collection:this.options.solutions});
      },

      render: function(){
       $(this.el).empty();
       $(this.el).append(this.solutionList.render().el);
       this.solutionList.show();
       this.solutionList.reposition();
       return this; 
      }

    });

  	return HomeView;
 });