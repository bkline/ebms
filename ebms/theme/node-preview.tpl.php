<?php
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
?>

<?php
switch ($node->type) {
    case 'ebms_event':
        break;
    default:
        print theme_node_preview($node);
        return;
}

// run through the preprocessor to generate the displayed values
$vars = array('node' => $node);
preprocess_ebms_event($vars);

?>
<div id='calendar-enclosure' class='preview'>

    <h2 class='indent trailing'> 
        <?php
        print $node->title;
        ?>
    </h2>
    <div class='indent trailing subheader'> <?php print $vars['eventDate']; ?> </div>
    <div class='indent trailing'> <?php print $vars['eventTime']; ?> </div>

    <div class='indent subheader'>Event Type</div>
    <div class='indent trailing'> <?php print $vars['eventType']; ?> </div>

    <?php if ($vars['boardName']) : ?>
        <div class='indent subheader'>Board</div>
        <div class='indent trailing'><?php print $vars['boardName']; ?></div>
        <?php
    endif;

    if ($vars['individuals']) :
        ?>
        <div class='indent subheader'>Individuals</div>
        <div class='indent trailing'> <?php print $vars['individuals']; ?> </div>
        <?php
    endif;

    if ($vars['agenda']) :
        ?>
        <div class='indent subheader'>Agenda</div>
        <div class='indent trailing'> <?php print $vars['agenda']; ?> </div>

        <?php
    endif;

    if ($vars['eventNotes']) :
        ?>
        <div class='indent subheader'>Notes</div>
        <div class='indent trailing'> <?php print $vars['eventNotes']; ?> </div>
    <?php endif; ?>

    <div class='indent trailing'> 
        <?php
        $submitter = $vars['submitter'];
        $submitted = $vars['submitted'];
        print "<i>Submitted by $submitter on $submitted</i>";
        ?> 
    </div>

    <?php if (!empty($vars['docLinks'])) : ?>
        <div class='trailing'>
            <div class='indent subheader'>Event Documents</div>

            <?php
            foreach ($vars['docLinks'] as $link)
                    print "<div class='indent'>$link</div>";
            ?>
        </div>
    <?php endif; ?>

</div>