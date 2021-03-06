<?php
class GalleryClass
{
  function GetGalleryList($set_id)
  {
    $resultRow = '';
    $result = mysql_query("select t.project_title, p.set_id, p.project_id, p.status, p.dorder from project p, project_title t where set_id = '$set_id' and t.lang_id = 1 and p.project_id = t.project_id order by p.dorder") or die (mysql_error());
    while($row = mysql_fetch_array($result))
    {
      $projectName = $row["project_title"];
      if(empty($row["project_title"]) || $row["project_title"] == null)
      {
        $projectName = '<No Name>';
      }

      $resultRow .= '<tr id = "'.$row["project_id"].'" setid = "'.$row["set_id"].'"><td >';
      $resultRow .= '<a href = "gallery_new.php?set_id='.$row["set_id"].'&project_id='.$row["project_id"].'">Project# '.$row["project_id"] . ' : ' .$projectName.'</a></td>';
      $resultRow .= '<td id = "dorder'.$row["project_id"].'">'.$row["dorder"].'</td>';
      $resultRow .= '<td>';
      if($row["status"] == 0)
      {
        $resultRow .= 'draft';
      }
      else {
        $resultRow .= 'published';
      }
      $resultRow .= '</td>';
      $resultRow .= '</tr>';
    }
    return $resultRow;
  }

  function InsertProject($set_id)
  {
    $result = mysql_query("select max(dorder) as newOrder from project where set_id = '$set_id'") or die (mysql_error());
    $value = mysql_fetch_object($result);
    $newOrder = $value->newOrder + 1;

    $result = mysql_query("insert into project (set_id, dorder, status) values ('$set_id', '$newOrder', 0)") or die (mysql_error());
    $project_id = mysql_insert_id();

    $result = mysql_query("insert into project_title (project_title, project_id, lang_id) select '', '$project_id', lang_id from lang_setting") or die (mysql_error());

    return $project_id;
  }

  function GetProjectPhotoID()
  {
    $photo = 1;
    $result = mysql_query('select max(project_photo_id) as photo_id from project_photo') or die (mysql_error());
    $value = mysql_fetch_object($result);
    $photo = $value->photo_id + 1;
    return $photo;
  }

  function SaveProjectPhoto($filePath, $project_id)
  {
    $sql = "select max(dorder) as newOrder from project_photo where project_id = '$project_id'";
    $result = mysql_query($sql) or die (mysql_error());
    $value = mysql_fetch_object($result);
    $dorder = $value->newOrder + 1;

    $sql = "insert into project_photo (photo_path, project_id, dorder) values ('$filePath', '$project_id', '$dorder')";
    $result = mysql_query($sql) or die (mysql_error());
    return mysql_insert_id();
  }

  function GetProjectImage($project_id, $set_id)
  {
    $resultRow .= '';

    $resultRow .= '<table data-toggle="table" id="photo_table" data-row-style="rowStyle" class="table table-hover">';
      $resultRow .= '<thead>';
      $resultRow .= '<tr>';
          $resultRow .= '<th>Photo</th>';
          $resultRow .= '<th>Display Order</th>';
          $resultRow .= '<th></th>';
      $resultRow .= '</tr>';
      $resultRow .= '</thead>';

      $reultRow .= '<tbody>';
    $result = mysql_query("select * from project_photo where project_id = '$project_id' order by dorder") or die (mysql_error());
    while($row = mysql_fetch_array($result))
    {
      $resultRow .= '<tr id = "'.$row["project_photo_id"].'">';
      $resultRow .= '<td><img src = "../'.$row["photo_path"].'" class = "img-responsive"/></td>';
      $resultRow .= '<td id = "dorder'.$row["project_photo_id"].'">'.$row["dorder"].'</td>';
      $resultRow .= '<td><a href = "project_photo_delete.php?project_photo_id='.$row["project_photo_id"].'&project_id='.$project_id.'&set_id='.$set_id.'">Delete</td></tr>';
    }
    $resultRow .= '</tbody>';
    $resultRow .= '</table>';
    return $resultRow;
  }

  function DeleteProjectPhoto($project_photo_id, $project_id, $set_id)
  {
    $result = mysql_query("select photo_path from project_photo where project_photo_id = '$project_photo_id'") or die (mysql_error());
    $photo_path = mysql_fetch_object($result)->photo_path;
    unlink('../'.$photo_path);

    $result = mysql_query("delete from project_photo where project_photo_id = '$project_photo_id'") or die (mysql_error());

    $result = mysql_query("select project_photo_id from project_photo where project_id = '$project_id' order by dorder") or die (mysql_error());
    $num = 1;
    while($row = mysql_fetch_array($result))
    {
      $photo_id = $row["project_photo_id"];
      mysql_query("update project_photo set dorder = '$num' where project_photo_id = '$photo_id'") or die (mysql_error());
      $num += 1;
    }
  }

  function GetProjectTitle($project_id)
  {
    $resultRow = '';
    $resultRow .= '<tr>';
    $result = mysql_query("select * from project_title t, lang_setting l where t.lang_id = l.lang_id and t.project_id = '$project_id'") or die (mysql_error());

    while($row = mysql_fetch_array($result))
    {
      $resultRow .= '<td>'.$row["display_name"].'<input type = "text" name = "project_title'.$row["project_title_id"].'" value = "'.$row["project_title"].'"/></td>';

    }
    $resultRow .= '</tr>';

    return $resultRow;
  }

  function GetProjectTitleIDArray($project_id)
  {
    $ProjectTitleIDArray = Array();
    $result = mysql_query("select * from project_title t, lang_setting l where t.lang_id = l.lang_id and t.project_id = '$project_id'") or die (mysql_error());

    while($row = mysql_fetch_array($result))
    {
      $ProjectTitleIDArray[] = $row["project_title_id"];

    }
    return $ProjectTitleIDArray;
  }

  function SaveProjectTitle($project_title, $project_id, $project_title_id)
  {
    $project_title = str_replace("'","\'", $project_title);
    mysql_query("update project_title set project_title = '$project_title' where project_id = '$project_id' and project_title_id = '$project_title_id'") or die (mysql_error());
  }

  function GetProjectSection($set_id, $project_id)
  {
    $resultRow = '';

    $result2 = mysql_query("select * from project_section_set where project_id ='$project_id'") or die (mysql_error());
    while($row2 = mysql_fetch_array($result2))
    {
      $section_set_id = $row2["section_set_id"];
      $resultRow .= '<div class = "row">';
      $resultRow .= '<div class = "col-md-12">';
      $resultRow .= '<h3>Section_ID: '.$section_set_id.'</h3>';
      $resultRow .= '<a href = "delete_section_set.php?section_set_id='.$section_set_id.'&set_id='.$set_id.'&project_id='.$project_id.'" class="btn btn-danger">Delete</a>';
      $resultRow .= '</div>';
      $resultRow .= '</div>';

      $sql = "select * from project_section s, lang_setting l where s.lang_id = l.lang_id and s.project_id = '$project_id' and s.section_set_id = '$section_set_id'";

      $result = mysql_query($sql ) or die (mysql_error());

      while($row = mysql_fetch_array($result))
      {
        $resultRow .= '<div class = "row">';
        $resultRow .= '<div class = "col-md-12">';
        $resultRow .= '<h4>'.$row["display_name"].' - Section</h4>';
        $resultRow .= '<b>Content</b>';
        $resultRow .= '<textarea name="section'.$row["project_section_id"].'" id="section'.$row["project_section_id"].'" rows="10" cols="80">';
        $resultRow .= $row["content"];
        $resultRow .= '</textarea>';
        $resultRow .= '<script>';
        $resultRow .= 'CKEDITOR.replace( "section'.$row["project_section_id"].'" );';
        $resultRow .= '</script>';
        $resultRow .= '</div>';
        $resultRow .= '</div>';
        $resultRow .= "<hr/>";
      }
    }

    $resultRow .= "<h3>New Section</h3>";

    $result = mysql_query("select * from lang_setting") or die(mysql_error());
    while($row = mysql_fetch_array($result))
    {
      $resultRow .= '<div class = "row">';
      $resultRow .= '<div class = "col-md-12">';
      $resultRow .= '<h4>'.$row["display_name"].' - New Section</h4>';
      $resultRow .= '<b>Content</b>';
      $resultRow .= '<textarea name="newSection'.$row["lang_id"].'" id="newSection'.$row["lang_id"].'" rows="10" cols="80">';
      $resultRow .= '</textarea>';
      $resultRow .= '<script>';
      $resultRow .= 'CKEDITOR.replace( "newSection'.$row["lang_id"].'" );';
      $resultRow .= '</script>';
      $resultRow .= '</div>';
      $resultRow .= '</div>';
      $resultRow .= "<hr/>";
    }
    return $resultRow;
  }

  function GetProjectSectionIDArray($project_id)
  {
    $ProjectSectionIDArray = Array();
    $result = mysql_query("select project_section_id from project_section where project_id = '$project_id'") or die (mysql_error());

    while($row = mysql_fetch_array($result))
    {
      $ProjectSectionIDArray[] = $row["project_section_id"];
    }
    return $ProjectSectionIDArray;
  }

  function InsertSection($project_id)
  {
    mysql_query("insert into project_section_set (project_id, create_date) values ('$project_id', NOW())") or die (mysql_error());
    return mysql_insert_id();
  }

  function DeleteSection($section_set_id)
  {
    mysql_query("delete from project_section where section_set_id = '.$section_set_id.'") or die (mysql_error());
    mysql_query("delete from project_section_set where section_set_id = '.$section_set_id.'") or die (mysql_error());
  }

  function InsertProjectSection($content, $project_id, $section_set_id, $lang_id)
  {
    $content = str_replace("'","\'", $content);
    mysql_query("insert into project_section (content, project_id, section_set_id, lang_id) values ('$content', '$project_id', '$section_set_id', '$lang_id')") or die (mysql_error());
  }

  function UpdateProjectSection($content, $project_section_id)
  {
    $content = str_replace("'","\'", $content);
    mysql_query("update project_section set content = '$content' where project_section_id = '$project_section_id'") or die (mysql_error());
  }

  function GetProject($currLang_ID)
  {
    $resultRow = '';
    $result = mysql_query("select set_id from gallery_category_setting where lang_id = '$currLang_ID' order by category_id") or die (mysql_error());
    while($row = mysql_fetch_array($result))
    {
      $set_id = $row["set_id"];

      if($_GET["allprojectson"] == 1)
      {
        $result2 = mysql_query("select project_id, seopath from project where set_id = '$set_id' order by dorder")  or die (mysql_error());
      }
      else {
        $result2 = mysql_query("select project_id, seopath from project where status = 1 and set_id = '$set_id' order by dorder")  or die (mysql_error());
      }

      while($row2 = mysql_fetch_array($result2))
      {
        $project_id = $row2["project_id"];
        $seopath = $row2["seopath"];
        $resultRow .= '<div class="col-sm-4 col-xs-12 isotopeSelector '.$set_id.'">';
            $resultRow .= '<article class="">';
                $resultRow .= '<figure>';



                    $resultFirstPhoto = mysql_query("select photo_path from project_photo where project_id = '$project_id' order by dorder limit 1")  or die (mysql_error());
                    $firstPhoto = mysql_fetch_object($resultFirstPhoto)->photo_path;
                    //$resultRow .= '<a href = "project_detail.php?project_id='.$project_id.'"><img src="/'.$firstPhoto.'"  style="height:240px;" /></a>';

                    if($currLang_ID != 1)
                    {
                      $langClass = new LangClass();
                      $lang_code = strtolower($langClass->GetLangCode($currLang_ID));
                      if($_GET["allprojectson"] == 1)
                      {
                        $resultRow .= '<a href = "/project_detail.php?project_id='.$project_id.'&lang='.$lang_code.'&allprojectson=1"><img src="/'.$firstPhoto.'"  style="height:240px;" /></a>';
                      }
                      else {
                        $resultRow .= '<a href = "/project/'.$seopath.'/'.$lang_code.'"><img src="/'.$firstPhoto.'"  style="height:240px;" /></a>';
                      }

                    }
                    else {
                      if($_GET["allprojectson"] == 1)
                      {
                        $resultRow .= '<a href = "/project_detail.php?project_id='.$project_id.'&allprojectson=1"><img src="/'.$firstPhoto.'"  style="height:240px;" /></a>';
                      }
                      else {
                        $resultRow .= '<a href = "/project/'.$seopath.'"><img src="/'.$firstPhoto.'"  style="height:240px;" /></a>';
                      }
                    }

                    /*$resultRow .= '<div class="overlay">';
                        $resultRow .= '<div class="inner-overlay">';
                            $resultRow .= '<div class="inner-overlay-content with-icons">';

                              $resultAllphoto = mysql_query("select photo_path from project_photo where project_id = '$project_id'") or die (mysql_error());
                              $countPhoto = 0;
                              while($allPhotoRow = mysql_fetch_array($resultAllphoto))
                              {
                                if($countPhoto == 0)
                                {
                                  $resultRow .= '<a class="fancybox-pop" rel="portfolio-2" href="'.$allPhotoRow["photo_path"].'">Demo';

                                  $resultRow .='</a>';
                                }
                                else
                                {
                                  $resultRow .= '<a class="fancybox-pop" style="display: none" href="'.$allPhotoRow["photo_path"].'" rel="portfolio-2"></a>';
                                }
                                $countPhoto +=1;
                              }

                                $resultRow .= '<a href="project_detail.php?project_id='.$project_id.'">Details</a>';
                            $resultRow .= '</div>';
                        $resultRow .= '</div>';
                    $resultRow .= '</div>';*/
                $resultRow .= '</figure>';

                $resultTitle = mysql_query("select project_title from project_title where project_id = '$project_id' and lang_id = '$currLang_ID'") or die (mysql_error());
                $resultRow .= '<div class="article-title">';

                if($currLang_ID != 1)
                {
                  $langClass = new LangClass();
                  $lang_code = strtolower($langClass->GetLangCode($currLang_ID));
                  if($_GET["allprojectson"] == 1)
                  {
                    $resultRow .= '<a href = "/project_detail.php?project_id='.$project_id.'&lang='.$lang_code.'&allprojectson=1">';
                  }
                  else {
                    $resultRow .= '<a href = "/project/'.$seopath.'/'.$lang_code.'">';
                  }

                }
                else {
                  if($_GET["allprojectson"] == 1)
                  {
                    $resultRow .= '<a href = "/project_detail.php?project_id='.$project_id.'&allprojectson=1">';
                  }
                  else {
                    $resultRow .= '<a href = "/project/'.$seopath.'">';
                  }
                }

                //'<a href="project_detail.php?project_id='.$project_id.'">';
                  $thisTitle = mysql_fetch_object($resultTitle)->project_title;
                  if(strripos($thisTitle, ",") > 0)
                  {
                    $subject = $thisTitle;
                    $search = ',';
                    $replace = ', <br/>';

                    $thisTitle = strrev(implode(strrev($replace), explode(strrev($search), strrev($subject), 2)));

                  }
                  else
                  {
                    $thisTitle = $thisTitle.'<br/>　';
                  }
                  $resultRow .= $thisTitle;
                $resultRow .= '</a></div>';

            $resultRow .= '</article>';
        $resultRow .= '</div>';

      }
    }
    return $resultRow;
  }

  function GalleryDetail($currLang_id, $project_id)
  {
    $resultSet = Array();
    $firstImageResult = '';
    $secondImageResult = '';

    $statusRs = mysql_query("select project_id from project where project_id = '$project_id' and status = 1") or die(mysql_error());
    $status = mysql_fetch_object($statusRs)->project_id;

    $allow = false;
    if($status > 0)
    {
      $allow = true;
    }

    if($_GET["allprojectson"] == "1")
    {
      $allow = true;
    }



    if($allow)
    {
      $result = mysql_query("select photo_path from project_photo where project_id = '$project_id' order by dorder") or die (mysql_error());
      $count = 0;
      while($row = mysql_fetch_array($result))
      {
        if($count == 0)
        {
          $firstImageResult .= '<li data-target="#myCarousel" data-slide-to="'.$count.'" class="active"></li>';
          $secondImageResult .= '<div class="item active"><img src="/'.$row["photo_path"].'"></div>';
        }
        else
        {
          $firstImageResult .= '<li data-target="#myCarousel" data-slide-to="'.$count.'"></li>';
          $secondImageResult .= '<div class="item"><img src="/'.$row["photo_path"].'"></div>';
        }
        $count++;
      }

      $resultRow = '';
      $resultTitle = mysql_query("select project_title from project_title where project_id = '$project_id' and lang_id = '$currLang_id'") or die (mysql_error());
      $resultRow .= '<h2>'.mysql_fetch_object($resultTitle)->project_title.'</h2>';

      $result = mysql_query("select content from project_section where project_id = '$project_id' and lang_id = '$currLang_id' order by project_section_id") or die (mysql_error());
      while($row = mysql_fetch_array($result))
      {
        $resultRow .= $row["content"];
      }

      $resultSet[] = $firstImageResult;
      $resultSet[] = $secondImageResult;
      $resultSet[] = $resultRow;
    }
    return $resultSet;
  }

  function GetGalleryStatus($project_id)
  {
    $resultRow = '';
    $result = mysql_query("select status from project where project_id = '$project_id'") or die (mysql_error());
    $status = mysql_fetch_object($result)->status;
    if($status == 1)
    {
        $resultRow .= '<input type = "hidden" name = "action" value = "0" />';
        $resultRow .= '<input type = "submit" value = "Set to Draft" class="btn btn-danger"/>';
    }
    else
    {
      $resultRow .= '<input type = "hidden" name = "action" value = "1" />';
      $resultRow .= '<input type = "submit" value = "Set to Publish" class="btn btn-primary"/>';
    }
    return $resultRow;
  }

  function SaveGalleryStatus($project_id, $action)
  {
    mysql_query("update project set status = '$action' where project_id = '$project_id'") or die (mysql_error());
  }

  function GetProjectSEOPath($project_id)
  {
    $resultRow = '';
    $result = mysql_query("select seopath from project where project_id = '$project_id'") or die (mysql_error());
    if($row = mysql_fetch_array($result))
    {
      $resultRow .= '<tr><td><input type = "text" name ="seopath" id ="seopath" value = "'.$row["seopath"].'" class="form-control"/></td></tr>';
    }
    return $resultRow;
  }

  function UpdateProjectSEOPath($project_id, $seopath)
  {
    $seopath = str_replace(",", "", $seopath);
    mysql_query("update project set seopath = '$seopath' where project_id = '$project_id'") or die (mysql_error());
  }

  function ProjectSEOPath($seopath)
  {
    $resultRow = '';
    $result = mysql_query("select project_id from project where seopath = '$seopath'") or die (mysql_error());
    if($row = mysql_fetch_array($result))
    {
      $resultRow .= $row["project_id"];
    }
    return $resultRow;
  }

  function UpdateDorder($project_id, $set_id, $dorder)
  {
    mysql_query("update project set dorder = '$dorder' where project_id = $project_id and set_id = '$set_id'") or die (mysql_error());
  }

  function UpdatePhotoDorder($project_photo_id, $project_id, $dorder)
  {
    mysql_query("update project_photo set dorder = '$dorder' where $project_photo_id = '$project_photo_id' and project_id = $project_id") or die (mysql_error());

  }
}

$galleryClass = new GalleryClass();

?>
