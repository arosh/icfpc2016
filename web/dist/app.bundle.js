!function(e){function t(r){if(n[r])return n[r].exports;var i=n[r]={exports:{},id:r,loaded:!1};return e[r].call(i.exports,i,i.exports,t),i.loaded=!0,i.exports}var n={};return t.m=e,t.c=n,t.p="",t(0)}([function(e,t,n){"use strict";var r=this&&this.__extends||function(e,t){function n(){this.constructor=e}for(var r in t)t.hasOwnProperty(r)&&(e[r]=t[r]);e.prototype=null===t?Object.create(t):(n.prototype=t.prototype,new n)},i=n(3),o=n(4),s=n(2),a=n(1),l=function(e){function t(t){e.call(this,t),this.store=new s.Store(this.onChange.bind(this)),this.state={inputText:"",vertex:[],edge:[]}}return r(t,e),t.prototype.render=function(){var e=function(e,t){return i.createElement("tr",{key:e},i.createElement("th",{scope:"row"},e),i.createElement("td",null,t))},t=this.state.vertex.map(function(t,n){return e("polygon["+n+"]",t.map(function(e){return"("+e.x+","+e.y+")"}).join(" "))}),n=this.state.edge.map(function(t,n){var r="("+t.st.x+","+t.st.y+")",i="("+t.en.x+","+t.en.y+")";return e("edge["+n+"]",r+" "+i)});return i.createElement("div",{className:"container"},i.createElement("div",{className:"row"},i.createElement("div",{className:"col-xs-6"},i.createElement("form",null,i.createElement("textarea",{rows:10,placeholder:"入力を貼り付けてね",value:this.state.inputText,onChange:this.onTextUpdate.bind(this),className:"form-control"}))),i.createElement("div",{id:"d3",className:"col-xs-6"})),i.createElement("div",{className:"row"},i.createElement("table",{className:"table table-condensed"},i.createElement("tbody",null,t,n))))},t.prototype.componentDidMount=function(){this.d3renderer=new a.D3Renderer("#d3")},t.prototype.onChange=function(){this.d3renderer.render(this.store.vertex,this.store.edge),this.setState({inputText:this.store.inputText,vertex:this.store.vertex,edge:this.store.edge})},t.prototype.onTextUpdate=function(e){this.store.onTextUpdate(e.target.value)},t}(i.Component);o.render(i.createElement(l,null),document.getElementById("app"))},function(e,t){"use strict";var n=555,r=50,i=5,o=function(){function e(){}return e.BLACK="#212121",e.RED="#F44336",e}(),s=function(){function e(e){this.el=d3.select(e).append("svg").attr({width:n,height:n}).classed("center-block",!0),this.skeletonEdgeEl=this.el.append("g"),this.silhouetteVertexEl=this.el.append("g"),this.silhouetteEdgeEl=this.el.append("g"),this.xAxisEl=this.el.append("g"),this.yAxisEl=this.el.append("g")}return e.prototype.render=function(e,t){this.computeScaler(e,t),this.renderSilhouetteVertex(e),this.renderSilhouetteEdge(e),this.renderSkeletonEdge(t),this.xAxisEl.call(d3.svg.axis().scale(this.xScale).orient("bottom")),this.yAxisEl.call(d3.svg.axis().scale(this.yScale).orient("right"))},e.prototype.renderSilhouetteVertex=function(e){for(var t=this,n=[],r=0,s=e;r<s.length;r++){var a=s[r];n.push.apply(n,a)}var l=this.silhouetteVertexEl.selectAll("circle").data(n);l.enter().append("circle"),l.exit().remove(),l.attr({cx:function(e,n){return t.xScale(e.x)},cy:function(e,n){return t.yScale(e.y)},fill:o.BLACK,r:i})},e.prototype.renderSilhouetteEdge=function(e){for(var t=this,n=[],r=0,i=e;r<i.length;r++)for(var s=i[r],a=0;a<s.length;++a){var l=(a+1)%s.length;n.push({st:s[a],en:s[l]})}var c=this.silhouetteEdgeEl.selectAll("line").data(n);c.enter().append("line"),c.exit().remove(),c.attr({stroke:o.BLACK,"stroke-width":2,x1:function(e,n){return t.xScale(e.st.x)},y1:function(e,n){return t.yScale(e.st.y)},x2:function(e,n){return t.xScale(e.en.x)},y2:function(e,n){return t.yScale(e.en.y)}})},e.prototype.renderSkeletonEdge=function(e){var t=this,n=this.skeletonEdgeEl.selectAll("line").data(e);n.enter().append("line"),n.exit().remove(),n.attr({stroke:o.RED,"stroke-width":2,x1:function(e,n){return t.xScale(e.st.x)},y1:function(e,n){return t.yScale(e.st.y)},x2:function(e,n){return t.xScale(e.en.x)},y2:function(e,n){return t.yScale(e.en.y)}})},e.prototype.computeScaler=function(e,t){for(var i=[],o=[],s=0,a=e;s<a.length;s++)for(var l=a[s],c=0,h=l;c<h.length;c++){var u=h[c];i.push(u.x),o.push(u.y)}for(var p=0,d=t;p<d.length;p++){var x=d[p];i.push(x.st.x,x.en.x),o.push(x.st.y,x.en.y)}var f=Math.min.apply(Math,i),y=Math.max.apply(Math,i),v=Math.min.apply(Math,o),g=Math.max.apply(Math,o),m=y-f,E=g-v;m>E?(this.xScale=d3.scale.linear().domain([f,y]).range([0+r,n-r]),this.yScale=d3.scale.linear().domain([(v+g)/2-m/2,(v+g)/2+m/2]).range([n-r,0+r])):(this.xScale=d3.scale.linear().domain([(f+y)/2-E/2,(f+y)/2+E/2]).range([0+r,n-r]),this.yScale=d3.scale.linear().domain([v,g]).range([n-r,0+r]))},e}();t.D3Renderer=s},function(e,t){"use strict";var n=function(){function e(e){this.tokens=e.trim().split(/\s+/),this.index=0}return e.prototype.hasNext=function(){return this.index<this.tokens.length},e.prototype.next=function(){return this.tokens[this.index++]},e.prototype.nextNumber=function(){return+this.next()},e.prototype.nextPoint=function(){var e=this.next().split(","),t=this.parseRational(e[0]),n=this.parseRational(e[1]);return{x:t,y:n}},e.prototype.parseRational=function(e){var t=e.split("/");return 1===t.length?+t[0]:2===t.length?+t[0]/+t[1]:NaN},e}(),r=function(){function e(e){this.onChange=e}return e.prototype.getErrorMessage=function(){return this.err},e.prototype.onTextUpdate=function(e){this.inputText=e;var t=new n(e);if(this.checkTokenError(t)!==!1){var r=t.nextNumber(),i=[];this.vertex=[];for(var o=0;o<r;++o){if(this.checkTokenError(t)===!1)return;var s=+t.next();i.push(s);for(var a=[],l=0;l<s;++l){if(this.checkTokenError(t)===!1)return;a.push(t.nextPoint())}this.vertex.push(a)}var c=t.nextNumber();this.edge=[];for(var o=0;o<c;++o){if(this.checkTokenError(t)===!1)return;var h=t.nextPoint(),u=t.nextPoint();this.edge.push({st:h,en:u})}this.onChange()}},e.prototype.checkTokenError=function(e){return e.hasNext()!==!1||(this.err=e.index+"th token",this.onChange(),!1)},e}();t.Store=r},function(e,t){e.exports=React},function(e,t){e.exports=ReactDOM}]);