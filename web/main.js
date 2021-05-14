
let app = new Vue({
  el: '#vue',
  data: {
    exe: '',
    cwd: '',
    env: [],
  },
  methods: {
    setData: function (data) {
      for (const [key, value] of Object.entries(data)) {
        this.$data[key] = value
      }
    }
  }
})

fetch('/response.json')
  .then(response => response.json())
  .then(data => app.setData(data))