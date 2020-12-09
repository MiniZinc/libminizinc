window.onload = function () {
  var to_link = document.getElementsByClassName("highlight-minizinc");
  for (var i = 0; i<to_link.length; i++) {
    if (to_link[i].hasAttribute('id')) {
      var newH6 = document.createElement('p');
      newH6.setAttribute('class','caption doc-link-caption');
      var newTag = document.createElement('a');
      newTag.setAttribute('href','#'+to_link[i].getAttribute('id'));
      newTag.setAttribute('class','headerlink');
      newTag.innerText = "¶";
      newH6.appendChild(newTag);
      to_link[i].parentNode.insertBefore(newH6, to_link[i]);
      newH6.appendChild(to_link[i]);
    }
  }
};
