$fn=30;

// https://www.desmos.com/calculator/ihjmltaprq?lang=de

/*
radiusB = 15; // base radius
incremH = 1;
incremA = 10;
incremL = 6.5; 
heightH = 150;
heightM = heightH - incremH;
powerP = 0.55; // larger = pointier
*/

/*
radiusB = 13; // base radius
incremH = 1;
incremA = 10;
incremL = 6.5; 
heightH = 145;
heightM = heightH - incremH;
powerP = 0.55; // larger = pointier
*/

/*
radiusB = 12.8; // base radius
incremH = 1;
incremA = 10;
incremL = 6.5; 
heightH = 145;
heightM = 15;
powerP = 0.55; // larger = pointier
*/

radiusB = 10.8; // base radius
incremH = 1;
incremA = 10;
incremL = 6.5; 
heightH = 145;
heightM = 15;
powerP = 0.55; // larger = pointier


minAngle0 = -105;
maxAngle0 = 105 - incremA;

function handleHeightA(
    h0,
    h1,
    radiusC0,
    radiusC1,
    centerO0,
    centerO1,
    angleCC0,
    angleCC1,
    centerAX0,
    centerAY0,
    centerAX1,
    centerAY1, 
    centerBX0,
    centerBY0,
    centerBX1,
    centerBY1
) = 
    [
        for ( angleA0 = [minAngle0 : incremA : maxAngle0] )
            
            let (vertexAX00 = centerAX0 + cos(angleCC0 + angleA0) * radiusC0)
            let (vertexAY00 = centerAY0 + sin(angleCC0 + angleA0) * radiusC0)
            let (vertexAX01 = centerAX0 + cos(angleCC0 + angleA0 + incremA) * radiusC0)
            let (vertexAY01 = centerAY0 + sin(angleCC0 + angleA0 + incremA) * radiusC0)
            
            let (vertexAX10 = centerAX1 + cos(angleCC1 + angleA0) * radiusC1)
            let (vertexAY10 = centerAY1 + sin(angleCC1 + angleA0) * radiusC1)
            let (vertexAX11 = centerAX1 + cos(angleCC1 + angleA0 + incremA) * radiusC1)
            let (vertexAY11 = centerAY1 + sin(angleCC1 + angleA0 + incremA) * radiusC1)
            
            [
                [vertexAX00,vertexAY00,h0],[vertexAX10,vertexAY10,h1],[vertexAX01,vertexAY01,h0],
                [vertexAX01,vertexAY01,h0],[vertexAX10,vertexAY10,h1],[vertexAX11,vertexAY11,h1]
            ]
 
    ];
        
function handleHeightB(
    h0,
    h1,
    radiusC0,
    radiusC1,
    centerO0,
    centerO1,
    angleCC0,
    angleCC1,
    centerAX0,
    centerAY0,
    centerAX1,
    centerAY1, 
    centerBX0,
    centerBY0,
    centerBX1,
    centerBY1
) = 
    [
        for ( angleB0 = [minAngle0 : incremA : (105 - incremA)] )
            
                let (vertexBX00 = centerBX0 + cos(angleCC0 + angleB0 + 180) * radiusC0)
                let (vertexBY00 = centerBY0 + sin(angleCC0 + angleB0 + 180) * radiusC0)
                let (vertexBX01 = centerBX0 + cos(angleCC0 + angleB0 + 180 + incremA) * radiusC0)
                let (vertexBY01 = centerBY0 + sin(angleCC0 + angleB0 + 180 + incremA) * radiusC0)
                
                let (vertexBX10 = centerBX1 + cos(angleCC1 + angleB0 + 180) * radiusC1)
                let (vertexBY10 = centerBY1 + sin(angleCC1 + angleB0 + 180) * radiusC1)
                let (vertexBX11 = centerBX1 + cos(angleCC1 + angleB0 + 180 + incremA) * radiusC1)
                let (vertexBY11 = centerBY1 + sin(angleCC1 + angleB0 + 180 + incremA) * radiusC1)
            
            [
                [vertexBX00,vertexBY00,h0],[vertexBX10,vertexBY10,h1],[vertexBX01,vertexBY01,h0],
                [vertexBX01,vertexBY01,h0],[vertexBX10,vertexBY10,h1],[vertexBX11,vertexBY11,h1]
            ]
 
    ];        

function handleHeight(h0, h1) =

    let (radiusC0 = pow((heightH - h0) * pow(radiusB, 1 / powerP) / heightH, powerP))
    let (radiusC1 = pow((heightH - h1) * pow(radiusB, 1 / powerP) / heightH, powerP))

    let (centerO0 = sin(15) * radiusC0)
    let (centerO1 = sin(15) * radiusC1)

    let (angleCC0 = h0 * incremL)
    let (angleCC1 = h1 * incremL)
    // echo(h, centerO, radiusC0);

    // semiprofile A
    let (centerAX0 = cos(angleCC0) * centerO0)
    let (centerAY0 = sin(angleCC0) * centerO0)
    let (centerAX1 = cos(angleCC1) * centerO1)
    let (centerAY1 = sin(angleCC1) * centerO1) 

    // semiprofile B
    let (centerBX0 = cos(angleCC0) * -centerO0)
    let (centerBY0 = sin(angleCC0) * -centerO0)
    let (centerBX1 = cos(angleCC1) * -centerO1)
    let (centerBY1 = sin(angleCC1) * -centerO1) 
    
    [
        handleHeightA(
            h0,
            h1,
            radiusC0,
            radiusC1,
            centerO0, 
            centerO1,
            angleCC0,
            angleCC1,
            centerAX0,
            centerAY0,
            centerAX1,
            centerAY1,
            centerBX0,
            centerBY0,
            centerBX1,
            centerBY1         
        ),
        handleHeightB(
            h0,
            h1,
            radiusC0,
            radiusC1,
            centerO0, 
            centerO1,
            angleCC0,
            angleCC1,
            centerAX0,
            centerAY0,
            centerAX1,
            centerAY1,
            centerBX0,
            centerBY0,
            centerBX1,
            centerBY1         
        )        
    ];

function iterateHeights() =
    [
    for ( h0 = [0 : incremH : heightM] ) // heightM
        let (h1 = h0 + incremH)
        handleHeight(h0, h1)
    ];
    

function flatten(l) = [ for (a = l) for (b = a) b ] ;
    
function indices(coordCount) =
    [
        for ( index = [0 : 3 : coordCount - 3] )
            [index, index + 1, index + 2]
    ];

module draw_horn() {
    
    points = flatten(flatten(flatten(iterateHeights()))); 
    // echo(points);
    
    triangles = indices(len(points));
    // echo(triangles);
    
    echo(len(points));
    
    polyhedron(points=points, triangles = triangles);    
    
} 

draw_horn();
