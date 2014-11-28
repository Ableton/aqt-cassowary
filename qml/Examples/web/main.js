
//c.trace = true
//c.debug = true

var canvas = document.getElementById('screen')

var solver = new c.SimplexSolver()
var centerY = new c.Variable()
var circleX = new c.Variable()
var circleY = new c.Variable()

var init = function () {
    solver
        .addEditVar(circleX, c.Strength.strong, 0)
        .addEditVar(circleY, c.Strength.strong, 0)
    solver
        .addConstraint(new c.Equation(
            c.times(2, centerY), canvas.height))
        .addConstraint(new c.Inequality(
            circleY, c.GEQ, centerY))
        .addConstraint(new c.Inequality(
            circleX, c.LEQ, canvas.width - 100))
        .addConstraint(new c.Inequality(
            circleX, c.GEQ, 100))
}

var redraw = function () {
    var context = canvas.getContext('2d')
    var radius = 10
    context.beginPath()
    context.arc(circleX.value, circleY.value, radius, 0, 2 * Math.PI, false)
    context.fillStyle = "rgba(64, 128, 200, 0.5)"
    context.fill()
    context.closePath()
}

var tick = function () {
    solver.suggestValue(circleX, Math.random() * canvas.width)
        .suggestValue(circleY, Math.random() * canvas.height)
        .resolve()
    redraw()
    requestAnimationFrame(tick)
}

init()
redraw()
requestAnimationFrame(tick)
