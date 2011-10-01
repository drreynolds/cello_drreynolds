<html>
<head>
<title>Enzo-P / Cello Unit Tests</title>
<link href="cello.css" rel="stylesheet" type="text/css">
   </head>
   <body>
   <h1>Enzo-P / Cello Unit Tests</h1>


   <?php

   //----------------------------------------------------------------------

   function test_group($testgroup) {
   printf ("<hr><h2>$testgroup Tests</h2><a name=\"$testgroup\">");
 }

   //----------------------------------------------------------------------

   function test_output ($output_file) {
     if (file_exists($output_file)) {
       $output_html = "<a href=\"$output_file\">$output.unit</a>";
       system("cat $output_file | awk 'BEGIN{c=0}; /UNIT TEST END/ {c=1}; END{ if (c==0) print \"<td class=fail><a href='$output_file'>incomplete</a></td>\"; if (c!=0) print \"<td class=pass><a href='$output_file'>complete</a></td>\"}'");
     } else {
       echo "<td></td>";
     }
   }

   //----------------------------------------------------------------------

function test_date ($output_file) {
  if (file_exists($output_file)) {
    $output_html = date ("Y-m-d", filemtime($output_file));
    echo "<td class=pass>$output_html</td>";
  } else {
    echo "<td class=pass></td>";
  }
}

   //----------------------------------------------------------------------

function test_time ($output_file) {
  if (file_exists($output_file)) {
    $output_html = date ("H:i:s", filemtime($output_file));
    echo "<td class=pass>$output_html</td>";
  } else {
    echo "<td class=pass></td>";
  }
}  

   //----------------------------------------------------------------------

function test_passed ($output_file) {
  if (file_exists($output_file)) {
    echo "<td class=pass>";
    system("cat $output_file | grep pass | grep '0/' | wc -l");
    echo "</td>";
  } else {
    echo "<td class=pass></td>";
  }
}

   //----------------------------------------------------------------------

function test_unfinished ($output_file) {
  if (file_exists($output_file)) {
    system("grep 0/ $output_file | awk 'BEGIN{c=0}; /incomplete/ {c=c+1}; END {if (c!=0) print\"<td class=unfinished>\",c,\"</td>\"; if (c==0) print \"<td class=pass>0</td>\"}'");
  } else {
    echo "<td class=pass></td>";
  }
}

   //----------------------------------------------------------------------

function test_failed ($output_file) {
  if (file_exists($output_file)) {
    system("grep 0/ $output_file | awk 'BEGIN{c=0}; /FAIL/ {c=c+1}; END {if (c!=0) print\"<td class=fail>\",c,\"</td>\"; if (c==0) print \"<td class=pass>0</td>\"}'");
  } else {
    echo "<td class=pass></td>";
  }
}

   //----------------------------------------------------------------------

function tests($component,$testrun,$output) {

  $parallel_types = array("serial","mpi","charm");

  $source_file = "../src/$component/$testrun.cpp";
  $source_html = "<a href=\"$source_file\">$testrun.cpp</a>";

  echo "<h3>Code: $source_html</br>Test: $output</h3>\n";
  if (! file_exists($source_file)) {
    echo "<p><strong class=fail>$source_file does not exist</strong></p>";
  } else {
    echo "<table>\n";


    echo "<tr>\n";
    echo "   <th></th>";
    echo "   <th>Output</th>";
    echo "   <th>Date</th>";
    echo "   <th>Time</th>";
    echo "   <th>Passed</th>";
    echo "   <th>Unfinished</th>";
    echo "   <th>Failed</th>";
    echo "</tr>\n";

    //--------------------------------------------------

    for ($i = 0; $i<sizeof($parallel_types); ++$i) {

      echo "<tr>\n";

      echo "<th> $parallel_types[$i] </th>";

      $output_file = "$parallel_types[$i]/$output.unit";

      test_output     ($output_file);
      test_date       ($output_file);
      test_time       ($output_file);
      test_passed     ($output_file);
      test_unfinished ($output_file);
      test_failed     ($output_file);

      echo "</tr>\n";
    }

    echo "</tr></table></br/>\n";

    echo "<table><tr>";

    for ($i = 0; $i<sizeof($parallel_types); ++$i) {
      $type = $parallel_types[$i];
      $output_file = "../test/$type/$output.unit";
      if (file_exists($output_file)) {
	test($type,$testrun,"FAIL");
      }
    }

    echo "</tr></table></br/>";
  }
};

   //----------------------------------------------------------------------

function test($type,$testrun,$type) {
  $ltype = strtolower($type);

  $cols = "$4,$6,$7,$8,$9,$10";
  $rowtext = "</tr><tr>";

  $output = "../test/$type/$testrun.unit";
  $count = exec("cat $output | grep $type | grep '0/' | wc -l");
  if ($count == 0) {
    #     echo "<strong >no ${ltype}ed tests</strong></br/>";
  } else {
    echo "<th class=$type><strong>$type ${ltype}ed</strong></th> ";
    system ("grep '0/' $output | sort | uniq | awk 'BEGIN {c=1}; / $type /{split($3,a,\"\/\"); print \"<td class=$type> \",$cols , \" </td>\"; c=c+1}; {if (c==5) {c=0; print \"$rowtext\"}}'");
    echo "</tr><tr></tr>";
  }
     
};

?>

<ul>
<li><a href="http://lca.ucsd.edu/projects/cello/">Cello User Site</a></li>
  <li><a href="http://client65-77.sdsc.edu/cello/">Cello Developer Site</a></li>

  </ul>


  <hr>

  <h2>Test Summary</h2>

  <?php

   //----------------------------------------------------------------------

  function summary_missing_executable ($parallel_types, $test_output, $executables)
{
  for ($i = 0; $i<sizeof($parallel_types); ++$i) {

    $type = $parallel_types[$i];

    $count_missing = 0;

    for ($test = 0; $test<sizeof($executables); ++$test) {
      $exe = $executables[$test];
      $bin = "../bin/$type/$exe";
      if (! file_exists($bin)) {
        ++ $count_missing ;
      }
    }

    if ($count_missing == 0) {
      printf ("<td></td>");
    } else {
      printf ("<td class=fail>$count_missing</td>");
    }
  }
  printf ("<th></th>");
}

   //----------------------------------------------------------------------

function summary_missing_output ($parallel_types, $test_output, $executables)
{
  for ($i = 0; $i<sizeof($parallel_types); ++$i) {

    $count_missing = 0;
    for ($test = 0; $test<sizeof($test_output); ++$test) {
      $output = "../test/$parallel_types[$i]/test_$test_output[$test].unit";
      if (! file_exists($output)) {
	++ $count_missing;
      }
    }
    if ($count_missing == 0) {
      printf ("<td></td>");
    } else {
      printf ("<td class=fail>$count_missing</td>");
    }
  }
  printf ("<th></th>");
}

   //----------------------------------------------------------------------

function summary_incomplete_output ($parallel_types, $test_output, $executables)
{
  for ($i = 0; $i<sizeof($parallel_types); ++$i) {

    $output_files = "";
    $num_output_files = 0;
    for ($test = 0; $test<sizeof($test_output); ++$test) {
      $output = "../test/$parallel_types[$i]/test_$test_output[$test].unit";
      $output_files = "$output_files $output";
      ++$num_output_files;
    }
    system("cat $output_files | awk 'BEGIN{b=\"$num_output_files\"; c=0}; /UNIT TEST BEGIN/ {c=c+1};/UNIT TEST END/ {b=b-1; c=c-1};END{if (c==0 && b<=0) {print \"<td></td>\"} else {print \"<td class=fail>\"b\"</td>\";}}'");
  }
  printf ("<th></th>");
}

   //----------------------------------------------------------------------

function summary_failed_tests ($parallel_types, $test_output, $executables)
{
  for ($i = 0; $i<sizeof($parallel_types); ++$i) {

    $output_files = "";
    for ($test = 0; $test<sizeof($test_output); ++$test) {
      $output = "../test/$parallel_types[$i]/test_$test_output[$test].unit";
      $output_files = "$output_files $output";
    }

    system("grep '0/' $output_files | awk 'BEGIN {c=0}; /FAIL/{c=c+1}; END{if (c==0) {print \"<td></td>\"} else {print \"<td class=fail>\",c,\"</td>\";}} '");
  }
  printf ("<th></th>");
}

   //----------------------------------------------------------------------

function summary_unfinished_tests ($parallel_types, $test_output, $executables)
{
  for ($i = 0; $i<sizeof($parallel_types); ++$i) {

    $output_files = "";
    for ($test = 0; $test<sizeof($test_output); ++$test) {
      $output = "../test/$parallel_types[$i]/test_$test_output[$test].unit";
      $output_files = "$output_files $output";
    }

    system("grep '0/' $output_files | awk 'BEGIN {c=0}; /incomplete/{c=c+1}; END{if (c==0) {print \"<td></td>\"} else {print \"<td class=unfinished>\",c,\"</td>\";}} '");
  }
  printf ("<th></th>");
}

   //----------------------------------------------------------------------

function summary_passed_tests ($parallel_types, $test_output, $executables)
{

  for ($i = 0; $i<sizeof($parallel_types); ++$i) {

    $output_files = "";
    for ($test = 0; $test<sizeof($test_output); ++$test) {
      $output = $test_output[$test];
      $output_files = "$output_files ../test/$parallel_types[$i]/test_$output.unit";
    }
    system("grep '0/' $output_files | awk 'BEGIN {c=0}; /pass/{c=c+1}; END{if (c==0) {print \"<td></td>\"} else {print \"<td class=pass>\",c,\"</td>\";}} '");

  }
  printf ("<th></th>");
}

   //----------------------------------------------------------------------

  function test_summary($component,$test_output,$executables)
{
  printf ("<tr><th><a href=\"#$component\">$component</a></th>\n");

  $parallel_types  = array("serial","mpi","charm");

  // Missing executable

  summary_missing_executable ($parallel_types,$test_output, $executables);
  summary_missing_output     ($parallel_types,$test_output, $executables);
  summary_incomplete_output     ($parallel_types,$test_output, $executables);
  summary_failed_tests      ($parallel_types,$test_output, $executables);
  summary_unfinished_tests ($parallel_types,$test_output, $executables);
  summary_passed_tests ($parallel_types,$test_output, $executables);

  printf ("</tr>\n");
}



printf ("<a href=serial/out.scons>SERIAL</a></br/>\n");
printf ("<a href=mpi/out.scons>   MPI</a></br/>\n");
printf ("<a href=charm/out.scons> CHARM</a></br/>\n");

printf ("<table>\n");
printf ("<tr>\n");
printf ( "<th rowspan=2>");
printf ("<strong>");
system("ls running.* | sed 's/running\.//g' | sed 's/\./\<\/br\/\> /g'");
printf ("</strong>");
printf ("</th>");
printf ( "<th colspan=3 class=fail>Missing Executable</th>");
printf ("<th></th>");
printf ( "<th colspan=3 class=fail>Missing Output</th>");
printf ("<th></th>");
printf ( "<th colspan=3 class=fail>Partial Output</th>");
printf ("<th></th>");
printf ( "<th colspan=3 class=fail>Failed Tests</th>");
printf ("<th></th>");
printf ( "<th colspan=3 class=unfinished>Unfinished Tests</th>");
printf ("<th></th>");
printf ( "<th colspan=3 class=pass>Passed Tests</th>");
printf ("<th></th>");
printf ( "</tr><tr>\n");

$parallel_labels = array("serial","mpi","charm");
for ($k = 0; $k < 6; $k ++) {
  for ($i = 0; $i < sizeof($parallel_labels); ++$i) {
    printf ("<th> $parallel_labels[$i] </th>");
  }
  printf ("<th> </th>");
}


test_summary("Enzo-PPM",
	     array("enzo-p_1",
		   "enzo-p_2"),
	     array("enzo-p",  "enzo-p"));

test_summary("Enzo-BC-2D", 
	     array("boundary-reflecting-2d",
		   "boundary-periodic-2d",
		   "boundary-inflow-2d",
		   "boundary-outflow-2d"),
	     array("enzo-p", "enzo-p", "enzo-p", "enzo-p"));

test_summary("Enzo-BC-3D",
	     array("boundary-reflecting-3d",
		   "boundary-periodic-3d",
		   "boundary-inflow-3d",
		   "boundary-outflow-3d"),
	     array("enzo-p", "enzo-p", "enzo-p", "enzo-p"));

// Print row divider

printf ("<tr><th></th>");
for ($k = 0; $k < 6; $k ++) {
  for ($i = 0; $i < sizeof($parallel_labels); ++$i) {
    printf ("<th> </th>");
  }
  printf ("<th></th>");
}
printf ("</tr>\n");


test_summary("Disk",array(     "FileHdf5",     "FileIfrit"),
	     array("test_FileHdf5","test_FileIfrit")); 
test_summary("Error",array(    "Error"),
	     array("test_Error")); 
test_summary("Enzo",array("enzo-p_1","enzo-p_2","boundary-reflecting-2d","boundary-periodic-2d","boundary-inflow-2d","boundary-outflow-2d","boundary-reflecting-3d","boundary-periodic-3d","boundary-inflow-3d","boundary-outflow-3d"),
	     array("enzo-p",  "enzo-p", "enzo-p", "enzo-p", "enzo-p", "enzo-p", "enzo-p", "enzo-p", "enzo-p", "enzo-p")); 
test_summary("Field",array(    "FieldBlock",     "FieldDescr",     "FieldFace",     "ItField"),
	     array("test_FieldBlock","test_FieldDescr","test_FieldFace","test_ItField")); 
test_summary("Io",array("ItReduce"),
	     array("test_ItReduce")); 
test_summary("Memory",array("Memory"),
	     array("test_Memory")); 
test_summary("Mesh",array("Hierarchy","Patch","Block"),
	     array("test_Hierarchy","test_Patch","test_Block")); 
test_summary("Monitor",array("Monitor"),
	     array("test_Monitor")); 
test_summary("Parallel",array("GroupProcess","Layout"),
	     array("test_GroupProcess","test_Layout")); 
test_summary("Parameters",array("Parameters"),
	     array("test_Parameters")); 
test_summary("Performance",array("Papi", "Performance"),
	     array("test_Papi","test_Performance")); 

printf ("</tr></table>\n");


test_group("Disk");

tests("Disk","test_FileHdf5", "test_FileHdf5");
tests("Disk","test_FileIfrit","test_FileIfrit");

test_group("Error");
tests("Error","test_Error","test_Error");


test_group("Field");
tests("Field","test_FieldDescr","test_FieldDescr");
tests("Field","test_FieldBlock","test_FieldBlock");
tests("Field","test_FieldFace","test_FieldFace");
tests("Field","test_ItField","test_ItField");

test_group("Io");
tests("Io","test_ItReduce", "test_ItReduce");

test_group("Memory");
tests("Memory","test_Memory","test_Memory");


test_group("Mesh");

tests("Mesh","test_Hierarchy","test_Hierarchy"); 
tests("Mesh","test_Patch","test_Patch"); 
tests("Mesh","test_Block","test_Block"); 

test_group("Method");

test_group("Monitor");
tests("Monitor","test_Monitor","test_Monitor");
printf ("<img src=\"monitor_image_1.png\"></img>\n");
printf ("<img src=\"monitor_image_2.png\"></img>\n");
printf ("<img src=\"monitor_image_3.png\"></img>\n");
printf ("<img src=\"monitor_image_4.png\"></img>\n");

test_group("Parallel");
tests("Parallel","test_GroupProcess","test_GroupProcess");
tests("Parallel","test_Layout","test_Layout");


test_group("Parameters");
tests("Parameters","test_Parameters","test_Parameters");

test_group("Particles");

test_group("Performance");
tests("Performance","test_Performance","test_Performance");
tests("Performance","test_Papi",       "test_Papi");

test_group("Portal");

test_group("Test");

test_group("Enzo");

?>


<h4>enzo-p</h4>

<?php tests("Enzo","enzo-p","test_enzo-p_1"); ?>
<?php tests("Enzo","enzo-p","test_enzo-p_2"); ?>

<table>
<tr>
<th></th>
<th colspan=3>density</th>
  <th colspan=3>total_energy</th>
  </tr>
  <tr>
  <th></th>
  <th>serial</th>
  <th>mpi</th>
  <th>charm</th>
  <th>serial</th>
  <th>mpi</th>
  <th>charm</th>
  </tr>
  <tr>
  <th>initial</th>
  <td><img width=96 src="serial/enzo-p_1-d-000000.png"></img></td>
  <td><img width=96 src="mpi/enzo-p_1-d-000000.png"></img></td>
  <td><img width=96 src="charm/enzo-p_1-d-000000.png"></img></td>
  <td><img width=96 src="serial/enzo-p_1-te-000000.png"></img></td>
  <td><img width=96 src="mpi/enzo-p_1-te-000000.png"></img></td>
  <td><img width=96 src="charm/enzo-p_1-te-000000.png"></img></td>
  </tr>
  <tr>
  <th>1 block cycle 100</th>
  <td><img width=96 src="serial/enzo-p_1-d-000100.png"></img></td>
  <td><img width=96 src="mpi/enzo-p_1-d-000100.png"></img></td>
  <td><img width=96 src="charm/enzo-p_1-d-000100.png"></img></td>
  <td><img width=96 src="serial/enzo-p_1-te-000100.png"></img></td>
  <td><img width=96 src="mpi/enzo-p_1-te-000100.png"></img></td>
  <td><img width=96 src="charm/enzo-p_1-te-000100.png"></img></td>
  </tr>
  <th>1 block cycle 200</th>
  <td><img width=96 src="serial/enzo-p_1-d-000200.png"></img></td>
  <td><img width=96 src="mpi/enzo-p_1-d-000200.png"></img></td>
  <td><img width=96 src="charm/enzo-p_1-d-000200.png"></img></td>
  <td><img width=96 src="serial/enzo-p_1-te-000200.png"></img></td>
  <td><img width=96 src="mpi/enzo-p_1-te-000200.png"></img></td>
  <td><img width=96 src="charm/enzo-p_1-te-000200.png"></img></td>
  </tr>
  <tr>
  <th>4 blocks cycle 100</th>
  <td><img width=96 src="serial/enzo-p_2-d-000100.png"></img></td>
  <td><img width=96 src="mpi/enzo-p_2-d-000100.png"></img></td>
  <td><img width=96 src="charm/enzo-p_2-d-000100.png"></img></td>
  <td><img width=96 src="serial/enzo-p_2-te-000100.png"></img></td>
  <td><img width=96 src="mpi/enzo-p_2-te-000100.png"></img></td>
  <td><img width=96 src="charm/enzo-p_2-te-000100.png"></img></td>
  </tr>
  <th>4 blocks cycle 200</th>
  <td><img width=96 src="serial/enzo-p_2-d-000200.png"></img></td>
  <td><img width=96 src="mpi/enzo-p_2-d-000200.png"></img></td>
  <td><img width=96 src="charm/enzo-p_2-d-000200.png"></img></td>
  <td><img width=96 src="serial/enzo-p_2-te-000200.png"></img></td>
  <td><img width=96 src="mpi/enzo-p_2-te-000200.png"></img></td>
  <td><img width=96 src="charm/enzo-p_2-te-000200.png"></img></td>
  </tr>
  </table>

  <h3> 2D Boundary conditions </h3>

  <?php tests("Enzo","enzo-p","test_boundary-reflecting-2d"); ?>
  <?php tests("Enzo","enzo-p","test_boundary-periodic-2d"); ?>
  <?php tests("Enzo","enzo-p","test_boundary-inflow-2d"); ?>
  <?php tests("Enzo","enzo-p","test_boundary-outflow-2d"); ?>

  <h4>2D Serial</h4>
  <table>
  <tr>
  <th></th>
  <th>000</th>
  <th>100</th>
  <th>200</th>
  <th>300</th>
  <th>400</th>
  <th>500</th>
  </tr>
  <tr>
  <th>reflecting</th>
  <td><img width=160 src="serial/boundary-reflecting-2d-0000.png"></img></td>
  <td><img width=160 src="serial/boundary-reflecting-2d-0100.png"></img></td>
  <td><img width=160 src="serial/boundary-reflecting-2d-0200.png"></img></td>
  <td><img width=160 src="serial/boundary-reflecting-2d-0300.png"></img></td>
  <td><img width=160 src="serial/boundary-reflecting-2d-0400.png"></img></td>
  <td><img width=160 src="serial/boundary-reflecting-2d-0500.png"></img></td>
  </tr>
  <tr>
  <th>periodic</th>
  <td><img width=160 src="serial/boundary-periodic-2d-0000.png"></img></td>
  <td><img width=160 src="serial/boundary-periodic-2d-0100.png"></img></td>
  <td><img width=160 src="serial/boundary-periodic-2d-0200.png"></img></td>
  <td><img width=160 src="serial/boundary-periodic-2d-0300.png"></img></td>
  <td><img width=160 src="serial/boundary-periodic-2d-0400.png"></img></td>
  <td><img width=160 src="serial/boundary-periodic-2d-0500.png"></img></td>
  </tr>
  <tr>
  <th>inflow</th>
  <td><img width=160 src="serial/boundary-inflow-2d-0000.png"></img></td>
  <td><img width=160 src="serial/boundary-inflow-2d-0100.png"></img></td>
  <td><img width=160 src="serial/boundary-inflow-2d-0200.png"></img></td>
  <td><img width=160 src="serial/boundary-inflow-2d-0300.png"></img></td>
  <td><img width=160 src="serial/boundary-inflow-2d-0400.png"></img></td>
  <td><img width=160 src="serial/boundary-inflow-2d-0500.png"></img></td>
  </tr>
  <tr>
  <th>outflow</th>
  <td><img width=160 src="serial/boundary-outflow-2d-0000.png"></img></td>
  <td><img width=160 src="serial/boundary-outflow-2d-0100.png"></img></td>
  <td><img width=160 src="serial/boundary-outflow-2d-0200.png"></img></td>
  <td><img width=160 src="serial/boundary-outflow-2d-0300.png"></img></td>
  <td><img width=160 src="serial/boundary-outflow-2d-0400.png"></img></td>
  <td><img width=160 src="serial/boundary-outflow-2d-0500.png"></img></td>
  </tr>
  </table>

  <h4>2D MPI</h4>
  <table>
  <tr>
  <th></th>
  <th>000</th>
  <th>100</th>
  <th>200</th>
  <th>300</th>
  <th>400</th>
  <th>500</th>
  </tr>
  <tr>
  <th>reflecting</th>
  <td><img width=160 src="mpi/boundary-reflecting-2d-0000.png"></img></td>
  <td><img width=160 src="mpi/boundary-reflecting-2d-0100.png"></img></td>
  <td><img width=160 src="mpi/boundary-reflecting-2d-0200.png"></img></td>
  <td><img width=160 src="mpi/boundary-reflecting-2d-0300.png"></img></td>
  <td><img width=160 src="mpi/boundary-reflecting-2d-0400.png"></img></td>
  <td><img width=160 src="mpi/boundary-reflecting-2d-0500.png"></img></td>
  </tr>
  <tr>
  <th>periodic</th>
  <td><img width=160 src="mpi/boundary-periodic-2d-0000.png"></img></td>
  <td><img width=160 src="mpi/boundary-periodic-2d-0100.png"></img></td>
  <td><img width=160 src="mpi/boundary-periodic-2d-0200.png"></img></td>
  <td><img width=160 src="mpi/boundary-periodic-2d-0300.png"></img></td>
  <td><img width=160 src="mpi/boundary-periodic-2d-0400.png"></img></td>
  <td><img width=160 src="mpi/boundary-periodic-2d-0500.png"></img></td>
  </tr>
  <tr>
  <th>inflow</th>
  <td><img width=160 src="mpi/boundary-inflow-2d-0000.png"></img></td>
  <td><img width=160 src="mpi/boundary-inflow-2d-0100.png"></img></td>
  <td><img width=160 src="mpi/boundary-inflow-2d-0200.png"></img></td>
  <td><img width=160 src="mpi/boundary-inflow-2d-0300.png"></img></td>
  <td><img width=160 src="mpi/boundary-inflow-2d-0400.png"></img></td>
  <td><img width=160 src="mpi/boundary-inflow-2d-0500.png"></img></td>
  </tr>
  <tr>
  <th>outflow</th>
  <td><img width=160 src="mpi/boundary-outflow-2d-0000.png"></img></td>
  <td><img width=160 src="mpi/boundary-outflow-2d-0100.png"></img></td>
  <td><img width=160 src="mpi/boundary-outflow-2d-0200.png"></img></td>
  <td><img width=160 src="mpi/boundary-outflow-2d-0300.png"></img></td>
  <td><img width=160 src="mpi/boundary-outflow-2d-0400.png"></img></td>
  <td><img width=160 src="mpi/boundary-outflow-2d-0500.png"></img></td>
  </tr>
  </table>

  <h4>2D CHARM++</h4>
  <table>
  <tr>
  <th></th>
  <th>000</th>
  <th>100</th>
  <th>200</th>
  <th>300</th>
  <th>400</th>
  <th>500</th>
  </tr>
  <tr>
  <th>reflecting</th>
  <td><img width=160 src="charm/boundary-reflecting-2d-0000.png"></img></td>
  <td><img width=160 src="charm/boundary-reflecting-2d-0100.png"></img></td>
  <td><img width=160 src="charm/boundary-reflecting-2d-0200.png"></img></td>
  <td><img width=160 src="charm/boundary-reflecting-2d-0300.png"></img></td>
  <td><img width=160 src="charm/boundary-reflecting-2d-0400.png"></img></td>
  <td><img width=160 src="charm/boundary-reflecting-2d-0500.png"></img></td>
  </tr>
  <tr>
  <th>periodic</th>
  <td><img width=160 src="charm/boundary-periodic-2d-0000.png"></img></td>
  <td><img width=160 src="charm/boundary-periodic-2d-0100.png"></img></td>
  <td><img width=160 src="charm/boundary-periodic-2d-0200.png"></img></td>
  <td><img width=160 src="charm/boundary-periodic-2d-0300.png"></img></td>
  <td><img width=160 src="charm/boundary-periodic-2d-0400.png"></img></td>
  <td><img width=160 src="charm/boundary-periodic-2d-0500.png"></img></td>
  </tr>
  <tr>
  <th>inflow</th>
  <td><img width=160 src="charm/boundary-inflow-2d-0000.png"></img></td>
  <td><img width=160 src="charm/boundary-inflow-2d-0100.png"></img></td>
  <td><img width=160 src="charm/boundary-inflow-2d-0200.png"></img></td>
  <td><img width=160 src="charm/boundary-inflow-2d-0300.png"></img></td>
  <td><img width=160 src="charm/boundary-inflow-2d-0400.png"></img></td>
  <td><img width=160 src="charm/boundary-inflow-2d-0500.png"></img></td>
  </tr>
  <tr>
  <th>outflow</th>
  <td><img width=160 src="charm/boundary-outflow-2d-0000.png"></img></td>
  <td><img width=160 src="charm/boundary-outflow-2d-0100.png"></img></td>
  <td><img width=160 src="charm/boundary-outflow-2d-0200.png"></img></td>
  <td><img width=160 src="charm/boundary-outflow-2d-0300.png"></img></td>
  <td><img width=160 src="charm/boundary-outflow-2d-0400.png"></img></td>
  <td><img width=160 src="charm/boundary-outflow-2d-0500.png"></img></td>
  </tr>
  </table>


  <h3> 3D Boundary conditions </h3>

  <?php tests("Enzo","enzo-p","test_boundary-reflecting-3d"); ?>
  <?php tests("Enzo","enzo-p","test_boundary-periodic-3d"); ?>
  <?php tests("Enzo","enzo-p","test_boundary-inflow-3d"); ?>
  <?php tests("Enzo","enzo-p","test_boundary-outflow-3d"); ?>

  <h4>3D Serial</h4>
  <table>
  <tr>
  <th></th>
  <th>00</th>
  <th>20</th>
  <th>40</th>
  <th>60</th>
  <th>80</th>
  </tr>
  <tr>
  <th>reflecting</th>
  <td><img width=192 src="serial/boundary-reflecting-3d-0000.png"></img></td>
  <td><img width=192 src="serial/boundary-reflecting-3d-0020.png"></img></td>
  <td><img width=192 src="serial/boundary-reflecting-3d-0040.png"></img></td>
  <td><img width=192 src="serial/boundary-reflecting-3d-0060.png"></img></td>
  <td><img width=192 src="serial/boundary-reflecting-3d-0080.png"></img></td>
  </tr>
  <tr>
  <th>periodic</th>
  <td><img width=192 src="serial/boundary-periodic-3d-0000.png"></img></td>
  <td><img width=192 src="serial/boundary-periodic-3d-0020.png"></img></td>
  <td><img width=192 src="serial/boundary-periodic-3d-0040.png"></img></td>
  <td><img width=192 src="serial/boundary-periodic-3d-0060.png"></img></td>
  <td><img width=192 src="serial/boundary-periodic-3d-0080.png"></img></td>
  </tr>
  <tr>
  <th>inflow</th>
  <td><img width=192 src="serial/boundary-inflow-3d-0000.png"></img></td>
  <td><img width=192 src="serial/boundary-inflow-3d-0020.png"></img></td>
  <td><img width=192 src="serial/boundary-inflow-3d-0040.png"></img></td>
  <td><img width=192 src="serial/boundary-inflow-3d-0060.png"></img></td>
  <td><img width=192 src="serial/boundary-inflow-3d-0080.png"></img></td>
  </tr>
  <tr>
  <th>outflow</th>
  <td><img width=192 src="serial/boundary-outflow-3d-0000.png"></img></td>
  <td><img width=192 src="serial/boundary-outflow-3d-0020.png"></img></td>
  <td><img width=192 src="serial/boundary-outflow-3d-0040.png"></img></td>
  <td><img width=192 src="serial/boundary-outflow-3d-0060.png"></img></td>
  <td><img width=192 src="serial/boundary-outflow-3d-0080.png"></img></td>
  </tr>
  </table>


  <h4>3D MPI</h4>
  <table>
  <tr>
  <th></th>
  <th>00</th>
  <th>20</th>
  <th>40</th>
  <th>60</th>
  <th>80</th>
  </tr>
  <tr>
  <th>reflecting</th>
  <td><img width=192 src="mpi/boundary-reflecting-3d-0000.png"></img></td>
  <td><img width=192 src="mpi/boundary-reflecting-3d-0020.png"></img></td>
  <td><img width=192 src="mpi/boundary-reflecting-3d-0040.png"></img></td>
  <td><img width=192 src="mpi/boundary-reflecting-3d-0060.png"></img></td>
  <td><img width=192 src="mpi/boundary-reflecting-3d-0080.png"></img></td>
  </tr>
  <tr>
  <th>periodic</th>
  <td><img width=192 src="mpi/boundary-periodic-3d-0000.png"></img></td>
  <td><img width=192 src="mpi/boundary-periodic-3d-0020.png"></img></td>
  <td><img width=192 src="mpi/boundary-periodic-3d-0040.png"></img></td>
  <td><img width=192 src="mpi/boundary-periodic-3d-0060.png"></img></td>
  <td><img width=192 src="mpi/boundary-periodic-3d-0080.png"></img></td>
  </tr>
  <tr>
  <th>inflow</th>
  <td><img width=192 src="mpi/boundary-inflow-3d-0000.png"></img></td>
  <td><img width=192 src="mpi/boundary-inflow-3d-0020.png"></img></td>
  <td><img width=192 src="mpi/boundary-inflow-3d-0040.png"></img></td>
  <td><img width=192 src="mpi/boundary-inflow-3d-0060.png"></img></td>
  <td><img width=192 src="mpi/boundary-inflow-3d-0080.png"></img></td>
  </tr>
  <tr>
  <th>outflow</th>
  <td><img width=192 src="mpi/boundary-outflow-3d-0000.png"></img></td>
  <td><img width=192 src="mpi/boundary-outflow-3d-0020.png"></img></td>
  <td><img width=192 src="mpi/boundary-outflow-3d-0040.png"></img></td>
  <td><img width=192 src="mpi/boundary-outflow-3d-0060.png"></img></td>
  <td><img width=192 src="mpi/boundary-outflow-3d-0080.png"></img></td>
  </tr>
  </table>


  <h4>3D CHARM++</h4>
  <table>
  <tr>
  <th></th>
  <th>00</th>
  <th>20</th>
  <th>40</th>
  <th>60</th>
  <th>80</th>
  </tr>
  <tr>
  <th>reflecting</th>
  <td><img width=192 src="charm/boundary-reflecting-3d-0000.png"></img></td>
  <td><img width=192 src="charm/boundary-reflecting-3d-0020.png"></img></td>
  <td><img width=192 src="charm/boundary-reflecting-3d-0040.png"></img></td>
  <td><img width=192 src="charm/boundary-reflecting-3d-0060.png"></img></td>
  <td><img width=192 src="charm/boundary-reflecting-3d-0080.png"></img></td>
  </tr>
  <tr>
  <th>periodic</th>
  <td><img width=192 src="charm/boundary-periodic-3d-0000.png"></img></td>
  <td><img width=192 src="charm/boundary-periodic-3d-0020.png"></img></td>
  <td><img width=192 src="charm/boundary-periodic-3d-0040.png"></img></td>
  <td><img width=192 src="charm/boundary-periodic-3d-0060.png"></img></td>
  <td><img width=192 src="charm/boundary-periodic-3d-0080.png"></img></td>
  </tr>
  <tr>
  <th>inflow</th>
  <td><img width=192 src="charm/boundary-inflow-3d-0000.png"></img></td>
  <td><img width=192 src="charm/boundary-inflow-3d-0020.png"></img></td>
  <td><img width=192 src="charm/boundary-inflow-3d-0040.png"></img></td>
  <td><img width=192 src="charm/boundary-inflow-3d-0060.png"></img></td>
  <td><img width=192 src="charm/boundary-inflow-3d-0080.png"></img></td>
  </tr>
  <tr>
  <th>outflow</th>
  <td><img width=192 src="charm/boundary-outflow-3d-0000.png"></img></td>
  <td><img width=192 src="charm/boundary-outflow-3d-0020.png"></img></td>
  <td><img width=192 src="charm/boundary-outflow-3d-0040.png"></img></td>
  <td><img width=192 src="charm/boundary-outflow-3d-0060.png"></img></td>
  <td><img width=192 src="charm/boundary-outflow-3d-0080.png"></img></td>
  </tr>
  </table>


  <h3>TreeK-D2-R2-L?</h3>

  <?php tests("Mesh","test_TreeK","test_TreeK-D2-R2-L6"); ?>
  <?php tests("Mesh","test_TreeK","test_TreeK-D2-R2-L7"); ?>
  <?php tests("Mesh","test_TreeK","test_TreeK-D2-R2-L8"); ?>
  <!--<?php tests("Mesh","test_TreeK","test_TreeK-D2-R2-L9"); ?> -->
<!--<?php tests("Mesh","test_TreeK","test_TreeK-D2-R2-L10"); ?> -->

<table>
<tr>
<th>coalesce</th>
<th>levels = 6</th>
  <th>levels = 7</th>
  <th>levels = 8</th>
  <!-- <th>levels = 9</th> -->
  <!-- <th>levels = 10</th> -->
  </tr>
  <tr>
  <th>false</th>
  <td><img width=257 src="serial/TreeK-D=2-R=2-L=6-0.png"></img></td>
  <td><img width=257 src="serial/TreeK-D=2-R=2-L=7-0.png"></img></td>
  <td><img width=257 src="serial/TreeK-D=2-R=2-L=8-0.png"></img></td>
  <!-- <td><img width=257 src="serial/TreeK-D=2-R=2-L=9-0.png"></img></td> -->
  <!-- <td><img width=257 src="serial/TreeK-D=2-R=2-L=10-0.png"></img></td> -->
  </tr>
  <tr>
  <th>true</th>
  <td><img width=257 src="serial/TreeK-D=2-R=2-L=6-1.png"></img></td>
  <td><img width=257 src="serial/TreeK-D=2-R=2-L=7-1.png"></img></td>
  <td><img width=257 src="serial/TreeK-D=2-R=2-L=8-1.png"></img></td>
  <!-- <td><img width=257 src="serial/TreeK-D=2-R=2-L=9-1.png"></img></td> -->
  <!-- <td><img width=257 src="serial/TreeK-D=2-R=2-L=10-1.png"></img></td> -->
  </tr>
  </table>

  <h3>TreeK-D2-R4-L?</h3>

  <?php tests("Mesh","test_TreeK","test_TreeK-D2-R4-L6"); ?>
  <?php tests("Mesh","test_TreeK","test_TreeK-D2-R4-L8"); ?>
  <!-- <?php tests("Mesh","test_TreeK","test_TreeK-D2-R4-L10"); ?> -->

<table>
<tr>
<th>coalesce</th>
<th>levels = 6</th>
  <th>levels = 8</th>
  <!-- <th>levels = 10</th> -->
  </tr>
  <tr>
  <th>false</th>
  <td><img width=257 src="serial/TreeK-D=2-R=4-L=6-0.png"></img></td>
  <td><img width=257 src="serial/TreeK-D=2-R=4-L=8-0.png"></img></td>
  <!-- <td><img width=257 src="serial/TreeK-D=2-R=4-L=10-0.png"></img></td> -->
  </tr>
  <tr>
  <th>true</th>
  <td><img width=257 src="serial/TreeK-D=2-R=4-L=6-1.png"></img></td>
  <td><img width=257 src="serial/TreeK-D=2-R=4-L=8-1.png"></img></td>
  <!-- <td><img width=257 src="serial/TreeK-D=2-R=4-L=10-1.png"></img></td> -->
  </tr>
  </table>

  <h3>TreeK-D3-R2-L?</h3>

  <?php tests("Mesh","test_TreeK","test_TreeK-D3-R2-L4"); ?>
  <?php tests("Mesh","test_TreeK","test_TreeK-D3-R2-L5"); ?>
  <?php tests("Mesh","test_TreeK","test_TreeK-D3-R2-L6"); ?>
  <!-- <?php tests("Mesh","test_TreeK","test_TreeK-D3-R2-L7"); ?> -->
<!-- <?php tests("Mesh","test_TreeK","test_TreeK-D3-R2-L8"); ?> -->

<table>
<tr>
<th>coalesce = false</th>
  <th>levels = 4</th>
  <th>levels = 5</th>
  <th>levels = 6</th>
  <!-- <th>levels = 7</th> -->
  <!-- <th>levels = 8</th> -->
  </tr>
  <tr>
  <th>project = X</th>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=4-x-0.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=5-x-0.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=6-x-0.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=7-x-0.png"></img></td> -->
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=8-x-0.png"></img></td> -->
  </tr>
  <tr>
  <th>project = Y</th>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=4-y-0.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=5-y-0.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=6-y-0.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=7-y-0.png"></img></td> -->
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=8-y-0.png"></img></td> -->
  </tr>
  <tr>
  <th>project = Z</th>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=4-z-0.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=5-z-0.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=6-z-0.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=7-z-0.png"></img></td> -->
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=8-z-0.png"></img></td> -->
  </tr>
  </table>

  <p>

  <table>
  <tr>
  <th>coalesce = true</th>
  <th>levels = 4</th>
  <th>levels = 5</th>
  <th>levels = 6</th>
  <!-- <th>levels = 7</th> -->
  <!-- <th>levels = 8</th> -->
  </tr>
  <tr>
  <th>project = X</th>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=4-x-1.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=5-x-1.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=6-x-1.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=7-x-1.png"></img></td> -->
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=8-x-1.png"></img></td> -->
  </tr>
  <tr>
  <th>project = Y</th>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=4-y-1.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=5-y-1.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=6-y-1.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=7-y-1.png"></img></td> -->
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=8-y-1.png"></img></td> -->
  </tr>
  <tr>
  <th>project = Z</th>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=4-z-1.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=5-z-1.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=2-L=6-z-1.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=7-z-1.png"></img></td> -->
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=2-L=8-z-1.png"></img></td> -->
  </tr>
  </table>


  <h3>TreeK-D3-R4-L?</h3>

  <?php tests("Mesh","test_TreeK","test_TreeK-D3-R4-L4"); ?>
  <?php tests("Mesh","test_TreeK","test_TreeK-D3-R4-L6"); ?>
  <!-- <?php tests("Mesh","test_TreeK","test_TreeK-D3-R4-L8"); ?> -->

<table>
<tr>
<th></th>
<th colspan=2>coalesce = false</th>
  <th colspan=2>coalesce = true</th>
  </tr>
  <tr>
  <th></th>
  <th>levels = 4</th>
  <th>levels = 6</th>
  <!-- <th>levels = 8</th> -->
  <th>levels = 4</th>
  <th>levels = 6</th>
  <!-- <th>levels = 8</th> -->
  </tr>
  <tr>
  <th>project = X</th>
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=4-x-0.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=6-x-0.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=4-L=8-x-0.png"></img></td> -->
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=4-x-1.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=6-x-1.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=4-L=8-x-1.png"></img></td> -->
  </tr>
  <tr>
  <th>project =  Y</th>
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=4-y-0.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=6-y-0.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=4-L=8-y-0.png"></img></td> -->
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=4-y-1.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=6-y-1.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=4-L=8-y-1.png"></img></td> -->
  </tr>
  <tr>
  <th>project = Z</th>
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=4-z-0.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=6-z-0.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=4-L=8-z-0.png"></img></td> -->
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=4-z-1.png"></img></td>
  <td><img width=129 src="serial/TreeK-D=3-R=4-L=6-z-1.png"></img></td>
  <!-- <td><img width=129 src="serial/TreeK-D=3-R=4-L=8-z-1.png"></img></td> -->
  </br/>
  </body>
  </html>
