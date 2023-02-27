$(document).ready(function(){
    const citationjs = require('citation-js');
    // Setup our bibliography. The $ element below gets expanded with the content of movetk.bib
    var bibliography = '@InProceedings{HHKPS01,author="Hert, Susanand Hoffmann, Michaeland Kettner, Lutzand Pion, Sylvainand Seel, Michael",editor="Brodal, Gerth St{\o}ltingand Frigioni, Danieleand Marchetti-Spaccamela, Alberto",title="An Adaptable and Extensible Geometry Kernel",booktitle="Algorithm Engineering",year="2001",publisher="Springer Berlin Heidelberg",address="Berlin, Heidelberg",pages="79--90",abstract="Geometric algorithms are based on geometric objects such as points, lines and circles. The term Kernel refers to a collection of representations for constant-size geometric objects and operations on these representations. This paper describes how such a geometry kernel can be designed and implemented in C++, having special emphasis on adaptability, extensibility and efficiency. We achieve these goals following the generic programming paradigm and using templates as our tools. These ideas are realized and tested in CGAL [9], the Computational Geometry Algorithms Library.",isbn="978-3-540-44688-0",doi ="https://doi.org/10.1007/3-540-44688-5_7"}@article{Fabri2000OnTD,    title={On the design of CGAL a computational geometry algorithms library},    author={Andreas Fabri and Geert-Jan Giezeman and Lutz Kettner and Stefan Schirra and Sven Sch{\"o}nherr},    journal={Softw., Pract. Exper.},    year={2000},    volume={30},    pages={1167-1202}}@article{BB2009,    title={A Generic Geometry Library},    author={Barend Gehrels and Bruno Lalande},    journal={BoostCon\'09},    year={2009}}@article{BBM2010,    title={Generic Programming for Geometry},    author={Barend Gehrels and Bruno Lalande and Mateusz Loskot},    journal={BoostCon\'10},    year={2010}}@article{alewijnse2018model,  title={Model-based segmentation and classification of trajectories},  author={Alewijnse, Sander PA and Buchin, Kevin and Buchin, Maike and Sijben, Stef and Westenberg, Michel A},  journal={Algorithmica},  volume={80},  pages={2422--2452},  year={2018},  publisher={Springer}}';
    
    // Replace all occurences of citations with links.
    const elements = document.getElementsByClassName('citationjs');

    let citation_count = 0;
    let citation_map = {};
    let citation_ids = []

    for (let i = 0; i < elements.length; i++) {
        let value = elements[i]
        const cite_id = value.dataset.citeid;
        // Find the index of the citation
        let index = 0;
        if(cite_id in citation_map){
            index = citation_map[cite_id];
        }else{
            index = citation_count+1
            citation_map[cite_id] = index;
            citation_count++;
            citation_ids.push(cite_id)
        }
        // Link to the right citation.
        value.getElementsByClassName('ref-container')[0].textContent = `[${index}]`;
    }
    
    // Output the bibliography
    if(citation_count > 0){
        // Construct the citation object
        let cite = citationjs.Cite(bibliography);
        const result = cite.format('bibliography', {
            format: 'html',
            template: 'apa',
            entry: citation_ids,
             prepend (entry) { return `[${citation_map[entry.id]}] ` } 
          });
        // Add the bibliography section
        $('.contents').append('<h1>Bibliography</h1>');
        $('.contents').append(result);
        // Make sure the citations are linked to the csl-entry elements.
        $('.csl-entry').each(function(index,e){
            let $elem = $(this);
            console.log(this);
            const id = $elem.data('csl-entry-id');
            $elem.attr('id',`cite-${id}`)
        });
    }
    
    document.getElementById('projectnumber').textContent = ' latest';
});
