"use strict";

//c.trace = true
//c.debug = true

var canvas = document.getElementById('screen')

var solver = new c.SimplexSolver()
var centerY = new c.Variable()
var circleX = new c.Variable()
var circleY = new c.Variable()
var marginX = new c.Variable({ value: 100 })

var init = function () {
    solver.autoSolve = false
    solver
        .addEditVar(circleX)
        .addEditVar(circleY)
    solver
        .addStay(marginX, c.Strength.strong)
    solver
        .addConstraint(new c.Equation(
            c.times(2, centerY), canvas.height))
        .addConstraint(new c.Inequality(
            circleY, c.GEQ, centerY))
        .addConstraint(new c.Inequality(
            circleY, c.LEQ, canvas.height))
        .addConstraint(new c.Inequality(
            circleX, c.LEQ, c.minus(canvas.width, marginX)))
        .addConstraint(new c.Inequality(
            circleX, c.GEQ, marginX))
    solver.beginEdit()
    solver.solve()
}

var redraw = function () {
    var context = canvas.getContext('2d')
    var radius = 10
    context.fillStyle = "rgba(64, 128, 200, 0.5)"
    context.clearRect(0, 0, canvas.width, canvas.height);
    context.beginPath()
    context.arc(circleX.value, circleY.value, radius, 0, 2 * Math.PI, false)
    context.fill()
    context.closePath()
    context.beginPath()
    context.rect(0, 0, canvas.width, centerY.value)
    context.fill()
    context.closePath()
    context.beginPath()
    context.rect(0, 0, marginX.value, canvas.height)
    context.rect(canvas.width - marginX.value, 0, marginX.value, canvas.height)
    context.fill()
    context.closePath()
}

var tick = function () {
    solver
        .suggestValue(circleX, Math.random() * canvas.width)
        .suggestValue(circleY, Math.random() * canvas.height)
        .resolve()
    redraw()
    requestAnimationFrame(tick)
}

function getMousePos(canvas, evt) {
    var rect = canvas.getBoundingClientRect()
    return {
        x: evt.clientX - rect.left,
        y: evt.clientY - rect.top
    }
}

canvas.addEventListener('mousemove', function(evt) {
    pos = getMousePos(canvas, evt)
    solver
        .suggestValue(circleX, pos.x)
        .suggestValue(circleY, pos.y)
        .resolve()
    redraw()
}, false);

init()
